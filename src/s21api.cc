#include <curl/curl.h>
#include <curl/easy.h>
#include <iostream>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/value.h>
#include <sstream>
#include <string>

#include "s21api.h"

using namespace std;

s21Api::s21Api(const string email, const string password)
    : email_(email), pass_(password), Campus(new Campus_t(this)),
      Cluster(new Cluster_t(this)), Event(new Event_t(this)),
      Sale(new Sale_t(this)), Project(new Project_t(this)),
      Coalition(new Coalition_t(this)), Participant(new Participant_t(this)),
      Course(new Course_t(this)), Graph(new Graph_t(this)) {

  GetTokenS();

  curl_ = curl_easy_init();
  if (!curl_) {
    throw runtime_error{"Curl initialiaztion failed"};
  }

  const string authorization = "Authorization: Bearer " + token_;
  headers_ = curl_slist_append(headers_, "accept: application/json");
  headers_ = curl_slist_append(headers_, authorization.c_str());
}

s21Api::s21Api(const string token)
    : token_(token), Campus(new Campus_t(this)), Cluster(new Cluster_t(this)),
      Event(new Event_t(this)), Sale(new Sale_t(this)),
      Project(new Project_t(this)), Coalition(new Coalition_t(this)),
      Participant(new Participant_t(this)), Course(new Course_t(this)),
      Graph(new Graph_t(this)) {

  curl_ = curl_easy_init();
  if (!curl_) {
    throw runtime_error{"Curl initialiaztion failed"};
  }

  const string authorization = "Authorization: Bearer " + token_;
  headers_ = curl_slist_append(headers_, "accept: application/json");
  headers_ = curl_slist_append(headers_, authorization.c_str());
}

s21Api::~s21Api() {
  delete Campus;
  delete Cluster;
  delete Event;
  delete Sale;
  delete Project;
  delete Coalition;
  delete Participant;
  delete Course;
  delete Graph;

  if (curl_) {
    curl_easy_cleanup(curl_);
  }
  if (headers_) {
    curl_slist_free_all(headers_);
  }
}

size_t s21Api::WriteCallback(void *contents, size_t size, size_t nmemb,
                             void *userp) {
  ((string *)userp)->append((char *)contents, size * nmemb);
  return size * nmemb;
}

void s21Api::GetTokenS() {
  curl_ = curl_easy_init();
  if (!curl_) {
    throw runtime_error{"GetToken(): Curl initialiaztion failed"};
  }

  string buff;
  const string getTokenUrl = "https://auth.sberclass.ru/auth/realms/"
                             "EduPowerKeycloak/protocol/openid-connect/token";
  const string postFields = "username=" + email_ + "&password=" + pass_ +
                            "&grant_type=password"
                            "&client_id=s21-open-api";

  struct curl_slist *header = nullptr;
  header = curl_slist_append(header,
                             "Content-Type: application/x-www-form-urlencoded");
  curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, header);
  curl_easy_setopt(curl_, CURLOPT_URL, getTokenUrl.c_str());
  curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, postFields.c_str());
  curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &buff);
  curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYPEER, 0L);
  curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYHOST, 0L);

  CURLcode res = curl_easy_perform(curl_);
  if (res != CURLE_OK) {
    cerr << "Error occurred while trying to get token: "
         << curl_easy_strerror(res);
  } else {
    Json::CharReaderBuilder charReader;
    Json::Value respJson;
    string errors;

    stringstream ss(buff);
    if (Json::parseFromStream(charReader, ss, &respJson, &errors)) {
      token_ = respJson["access_token"].asString();
    } else {
      cerr << "Failed to parse Json repsponse" << endl;
    }
  }

  curl_slist_free_all(header);
  curl_easy_cleanup(curl_);
}

string s21Api::SendRequest(const string &path) {
  string buff;
  string fullPath = baseUrl_ + path;

  curl_easy_setopt(curl_, CURLOPT_URL, fullPath.c_str());
  curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headers_);
  curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(curl_, CURLOPT_WRITEDATA, &buff);
  curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYPEER, 0L);
  curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYHOST, 0L);

  CURLcode res = curl_easy_perform(curl_);
  if (res != CURLE_OK) {
    cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res);
  }

  return buff;
}

void s21Api::ParamsToStr(string &s, const int64_t limit, const int64_t offset,
                         bool occupied, const int type, const int status) {
  if (limit > 0 || offset > 0 || occupied || ((type > 0) && (type < 4)) ||
      ((status > 0) && (status < 7))) {
    auto pos = s.rfind('?');

    if (pos == s.npos) {
      s += "?";
    } else {
      s += "&";
    }

    if (limit) {
      s += "limit=" + ((limit <= 1000) ? to_string(limit) : string("1000")) +
           ((offset || occupied || type || status) ? "&" : "");
    }
    if (offset) {
      s += "offset=" + to_string(offset) +
           ((occupied || type || status) ? "&" : "");
    }
    if (occupied) {
      s += "occupied=1" + string((type || status) ? "&" : "");
    }
    if (type) {
      s += "type=" + string((type == ACTIVITY) ? "ACTIVITY"
                            : (type == EXAM)   ? "EXAM"
                                               : "TEST");
    }
    if (status) {
      string statusStr = (status == ASSIGNED)      ? "ASSIGNED"
                         : (status == REGISTERED)  ? "REGISTERED"
                         : (status == IN_PROGRESS) ? "IN_PROGRESS"
                         : (status == IN_REVIEWS)  ? "IN_REVIEWS"
                         : (status == ACCEPTED)    ? "ACCEPTED"
                                                   : "FAILED";
      s += "status=" + statusStr;
    }
  }
}

const string s21Api::GetToken() const { return token_; }

/////////////////////////CAMPUS//////////////////////////////////////

const string s21Api::Campus_t::campuses() {
  return parrent_->SendRequest("/v1/campuses");
}

const string s21Api::Campus_t::participants(const std::string campusId,
                                            const int64_t limit,
                                            const int64_t offset) {
  if (campusId.empty()) {
    return "{\"Error\": \"Campus ID required.\"}";
  }

  string reqPath{"/v1/campuses/" + campusId + "/participants"};
  ParamsToStr(reqPath, limit, offset);

  return parrent_->SendRequest(reqPath);
}

const string s21Api::Campus_t::coalitions(const std::string campusId,
                                          const int32_t limit,
                                          const int32_t offset) {
  if (campusId.empty()) {
    return "{\"Error\": \"Campus ID required.\"}";
  }

  string reqPath{"/v1/campuses/" + campusId + "/coalitions"};
  ParamsToStr(reqPath, limit, offset);

  return parrent_->SendRequest(reqPath);
}

const string s21Api::Campus_t::clusters(const string campusId) {
  if (campusId.empty()) {
    return "{\"Error\": \"Campus ID required.\"}";
  }

  return parrent_->SendRequest("/v1/campuses/" + campusId + "/clusters");
}

////////////////////////////CLUSTER//////////////////////////////////

const string s21Api::Cluster_t::Map(const int64_t clusterId,
                                    const int32_t limit, const int32_t offset,
                                    const bool occupied) {
  string reqPath = "/v1/clusters/" + to_string(clusterId) + "/map";
  ParamsToStr(reqPath, limit, offset, occupied);

  return parrent_->SendRequest(reqPath);
}

////////////////////////////////EVENT///////////////////////////////

const string s21Api::Event_t::events(const std::string &from,
                                     const std::string &to, const int type,
                                     const int64_t limit,
                                     const int64_t offset) {
  if (from.empty() || to.empty()) {
    return "{\"Error\": \"from & to are required\"}";
  }

  string reqPath = "/v1/events?from=" + from + "&to=" + to;
  ParamsToStr(reqPath, limit, offset, false, type);

  return parrent_->SendRequest(reqPath);
}

///////////////////////////////SALE/////////////////////////////////

const string s21Api::Sale_t::sales() {
  return parrent_->SendRequest("/v1/sales");
}

///////////////////////////PROJECT//////////////////////////////////

const string s21Api::Project_t::projects(const int64_t projectId) {
  return parrent_->SendRequest("/v1/projects/" + to_string(projectId));
}

const string s21Api::Project_t::participants(const int64_t projectId,
                                             const int64_t limit,
                                             const int64_t offset,
                                             const int status,
                                             const std::string campusId) {
  string reqPath = "/v1/projects/" + to_string(projectId) + "/participants";

  ParamsToStr(reqPath, limit, offset, 0, 0, status);

  if (!campusId.empty()) {
    reqPath += "&campusId=" + campusId;
  }

  return parrent_->SendRequest(reqPath);
}

///////////////////////////////COALITION//////////////////////////////////

const string s21Api::Coalition_t::participant(const int64_t coalitionId,
                                              const int32_t limit,
                                              const int32_t offset) {
  string reqPath = "/v1/coalitions/" + to_string(coalitionId) + "/participants";
  ParamsToStr(reqPath, limit, offset);

  return parrent_->SendRequest(reqPath);
}

////////////////////////////////PARTICIPANT///////////////////////////////

const string s21Api::Participant_t::participants(const string &login,
                                                 bool withMailDomen) {
  if (login.empty()) {
    return "{\"Error\": \"Login is required.\"}";
  }

  if (withMailDomen) {
    string *p = const_cast<string *>(&login);
    MakeWholeName(p);
  }

  return parrent_->SendRequest("/v1/participants/" + login);
}

const string s21Api::Participant_t::workstation(const string &login,
                                                bool withMailDomen) {
  if (login.empty()) {
    return "{\"Error\": \"Login is required.\"}";
  }

  if (withMailDomen) {
    string *p = const_cast<string *>(&login);
    MakeWholeName(p);
  }

  return parrent_->SendRequest("/v1/participants/" + login + "/workstation");
}

const string s21Api::Participant_t::skills(const string &login,
                                           bool withMailDomen) {
  if (login.empty()) {
    return "{\"Error\": \"Login is required.\"}";
  }

  if (withMailDomen) {
    string *p = const_cast<string *>(&login);
    MakeWholeName(p);
  }

  return parrent_->SendRequest("/v1/participants/" + login + "/skills");
}

const string s21Api::Participant_t::projects(const std::string &login,
                                             const int64_t limit,
                                             const int64_t offset,
                                             const int status,
                                             bool withMailDomen) {
  if (login.empty()) {
    return "{\"Error\": \"Login is required.\"}";
  }

  if (withMailDomen) {
    string *p = const_cast<string *>(&login);
    MakeWholeName(p);
  }

  string reqPath = "/v1/participants/" + login + "/projects";
  ParamsToStr(reqPath, limit, offset, 0, 0, status);

  return parrent_->SendRequest(reqPath);
}

const string s21Api::Participant_t::projects(const std::string &login,
                                             const int64_t projectId,
                                             bool withMailDomen) {
  if (login.empty() || projectId < 1) {
    return "{\"Error\": \"Login & project ID are required.\"}";
  }

  if (withMailDomen) {
    string *p = const_cast<string *>(&login);
    MakeWholeName(p);
  }

  return parrent_->SendRequest("/v1/participants/" + login + "/projects/" +
                               to_string(projectId));
}

const string s21Api::Participant_t::points(const string &login,
                                           bool withMailDomen) {
  if (login.empty()) {
    return "{\"Error\": \"Login is required.\"}";
  }

  if (withMailDomen) {
    string *p = const_cast<string *>(&login);
    MakeWholeName(p);
  }

  return parrent_->SendRequest("/v1/participants/" + login + "/points");
}

const string s21Api::Participant_t::logtime(const std::string &login,
                                            const std::string date,
                                            bool withMailDomen) {
  if (login.empty()) {
    return "{\"Error\": \"Login is required.\"}";
  }

  if (withMailDomen) {
    string *p = const_cast<string *>(&login);
    MakeWholeName(p);
  }

  string reqPath = "/v1/participants/" + login + "/logtime" +
                   ((date.empty()) ? "" : ("?date=" + date));

  return parrent_->SendRequest(reqPath);
}

const string s21Api::Participant_t::feedback(const string &login,
                                             bool withMailDomen) {
  if (login.empty()) {
    return "{\"Error\": \"Login is required.\"}";
  }

  if (withMailDomen) {
    string *p = const_cast<string *>(&login);
    MakeWholeName(p);
  }

  return parrent_->SendRequest("/v1/participants/" + login + "/feedback");
}

const string s21Api::Participant_t::experience_history(const std::string &login,
                                                       const int64_t limit,
                                                       const int64_t offset,
                                                       bool withMailDomen) {
  if (login.empty()) {
    return "{\"Error\": \"Login is required.\"}";
  }

  if (withMailDomen) {
    string *p = const_cast<string *>(&login);
    MakeWholeName(p);
  }

  string reqPath = "/v1/participants/" + login + "/experience-history";
  ParamsToStr(reqPath, limit, offset);

  return parrent_->SendRequest(reqPath);
}

const string s21Api::Participant_t::courses(const std::string &login,
                                            const int64_t limit,
                                            const int64_t offset,
                                            const int status,
                                            bool withMailDomen) {
  if (login.empty()) {
    return "{\"Error\": \"Login is required.\"}";
  }

  if (withMailDomen) {
    string *p = const_cast<string *>(&login);
    MakeWholeName(p);
  }

  string reqPath = "/v1/participants/" + login + "/courses";
  ParamsToStr(reqPath, limit, offset, 0, 0, status);

  return parrent_->SendRequest(reqPath);
}

const string s21Api::Participant_t::courses(const std::string &login,
                                            const int64_t courseId,
                                            bool withMailDomen) {
  if (login.empty() || courseId < 1) {
    return "{\"Error\": \"Login & course ID are required.\"}";
  }

  if (withMailDomen) {
    string *p = const_cast<string *>(&login);
    MakeWholeName(p);
  }

  return parrent_->SendRequest("/v1/participants/" + login + "/courses/" +
                               to_string(courseId));
}

const string s21Api::Participant_t::coalition(const string &login,
                                              bool withMailDomen) {
  if (login.empty()) {
    return "{\"Error\": \"Login is required.\"}";
  }

  if (withMailDomen) {
    string *p = const_cast<string *>(&login);
    MakeWholeName(p);
  }

  return parrent_->SendRequest("/v1/participants/" + login + "/coalition");
}

const string s21Api::Participant_t::badges(const string &login,
                                           bool withMailDomen) {
  if (login.empty()) {
    return "{\"Error\": \"Login is required.\"}";
  }

  if (withMailDomen) {
    string *p = const_cast<string *>(&login);
    MakeWholeName(p);
  }

  return parrent_->SendRequest("/v1/participants/" + login + "/badges");
}

void s21Api::Participant_t::MakeWholeName(string *login) {
  auto pos = login->find('@');
  if (pos == login->npos) {
    *login += "@student.21-school.ru";
  }
}

////////////////////////////COURSE////////////////////////////////////////

const string s21Api::Course_t::courses(const int64_t courseId) {
  return parrent_->SendRequest("/v1/courses/" + to_string(courseId));
}

////////////////////////////GRAPH/////////////////////////////////////////

const string s21Api::Graph_t::graph() {
  return parrent_->SendRequest("/v1/graph");
}
