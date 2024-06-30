#ifndef SRC_S21_API_H
#define SRC_S21_API_H

#include <cstdint>
#include <curl/curl.h>
#include <curl/easy.h>
#include <string>

enum Type { ACTIVITY = 1, EXAM, TEST };

enum Status {
  ASSIGNED = 1,
  REGISTERED,
  IN_PROGRESS,
  IN_REVIEWS,
  ACCEPTED,
  FAILED
};

class s21Api {
  struct Campus_t;
  struct Cluster_t;
  struct Event_t;
  struct Sale_t;
  struct Project_t;
  struct Coalition_t;
  struct Participant_t;
  struct Course_t;
  struct Graph_t;

public:
  s21Api() = delete;
  s21Api(const s21Api &) = delete;
  s21Api operator=(const s21Api &) = delete;
  s21Api(s21Api &&) = delete;
  s21Api &operator=(s21Api &&) = delete;

  explicit s21Api(const std::string email, const std::string password);
  explicit s21Api(const std::string token);

  ~s21Api();

  const std::string GetToken() const;

  Campus_t *Campus;
  Cluster_t *Cluster;
  Event_t *Event;
  Sale_t *Sale;
  Project_t *Project;
  Coalition_t *Coalition;
  Participant_t *Participant;
  Course_t *Course;
  Graph_t *Graph;

private:
  static size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                              void *userp);
  void GetTokenS();
  std::string SendRequest(const std::string &path);
  static void ParamsToStr(std::string &s, const int64_t limit = 0,
                          const int64_t offset = 0, bool occupied = false,
                          const int type = 0, const int status = 0);

  ////////////////////////////////////////////////////////////////////

  struct Campus_t {
    Campus_t(s21Api *p) : parrent_(p) {}
    const std::string campuses();
    const std::string participants(const std::string campusId,
                                   const int64_t limit = 0,
                                   const int64_t offset = 0);
    const std::string coalitions(const std::string campusId,
                                 const int32_t limit = 0,
                                 const int32_t offset = 0);
    const std::string clusters(const std::string campusId);

  private:
    s21Api *parrent_;
  };

  struct Cluster_t {
    Cluster_t(s21Api *p) : parrent_(p) {}
    const std::string Map(const int64_t clusterID, const int32_t limit = 0,
                          const int32_t offset = 0,
                          const bool occupied = false);

  private:
    s21Api *parrent_;
  };

  struct Event_t {
    Event_t(s21Api *p) : parrent_(p) {}
    const std::string events(const std::string &from, const std::string &to,
                             const int type = 0, const int64_t limit = 0,
                             const int64_t offset = 0);

  private:
    s21Api *parrent_;
  };

  struct Sale_t {
    Sale_t(s21Api *p) : parrent_(p) {}
    const std::string sales();

  private:
    s21Api *parrent_;
  };

  struct Project_t {
    Project_t(s21Api *p) : parrent_(p) {}
    const std::string projects(const int64_t projectId);
    const std::string participants(const int64_t projectId,
                                   const int64_t limit = 0,
                                   const int64_t offset = 0,
                                   const int status = 0,
                                   const std::string campusId = "");

  private:
    s21Api *parrent_;
  };

  struct Coalition_t {
    Coalition_t(s21Api *p) : parrent_(p) {}
    const std::string participant(const int64_t coalitionId,
                                  const int32_t limit = 0,
                                  const int32_t offset = 0);

  private:
    s21Api *parrent_;
  };

  struct Participant_t {
    Participant_t(s21Api *p) : parrent_(p) {}
    const std::string participants(const std::string &login,
                                   bool withMailDomen = true);
    const std::string workstation(const std::string &login,
                                  bool withMailDomen = true);
    const std::string skills(const std::string &login,
                             bool withMailDomen = true);
    const std::string projects(const std::string &login, const int64_t limit,
                               const int64_t offset, const int status,
                               bool withMailDomen = true);
    const std::string projects(const std::string &login,
                               const int64_t projectId,
                               bool withMailDomen = true);
    const std::string points(const std::string &login,
                             bool withMailDomen = true);
    const std::string logtime(const std::string &login,
                              const std::string date = "",
                              bool withMailDomen = true);
    const std::string feedback(const std::string &login,
                               bool withMailDomen = true);
    const std::string experience_history(const std::string &login,
                                         const int64_t limit = 0,
                                         const int64_t offset = 0,
                                         bool withMailDomen = true);
    const std::string courses(const std::string &login, const int64_t limit,
                              const int64_t offset, const int status,
                              bool withMailDomen = true);
    const std::string courses(const std::string &login, const int64_t courseId,
                              bool withMailDomen = true);
    const std::string coalition(const std::string &login,
                                bool withMailDomen = true);
    const std::string badges(const std::string &login,
                             bool withMailDomen = true);

  private:
    void MakeWholeName(std::string *login);
    s21Api *parrent_;
  };

  struct Course_t {
    Course_t(s21Api *p) : parrent_(p) {}
    const std::string courses(const int64_t courseId);

  private:
    s21Api *parrent_;
  };

  struct Graph_t {
    Graph_t(s21Api *p) : parrent_(p) {}
    const std::string graph();

  private:
    s21Api *parrent_;
  };

  ////////////////////////////////////////////////////////////////////

  CURL *curl_;
  struct curl_slist *headers_ = nullptr;
  const std::string baseUrl_ =
      "https://edu-api.21-school.ru/services/21-school/api";
  std::string token_;
  std::string email_;
  std::string pass_;
};

#endif // SRC_S21_API_H
