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
  s21Api(s21Api &o) = delete;
  s21Api(s21Api &&o) = delete;
  explicit s21Api(const std::string email, const std::string password);
  ~s21Api();

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
  void GetToken();
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
  std::string token_ =
      "eyJhbGciOiJSUzI1NiIsInR5cCIgOiAiSldUIiwia2lkIiA6ICJ5V29landCTmxROWtQVEpF"
      "ZnFpVzRrc181Mk1KTWkwUHl2RHNKNlgzdlFZIn0."
      "eyJleHAiOjE3MTk3NjA0MzQsImlhdCI6MTcxOTcyNDQzNCwianRpIjoiNmRlYzUyMjMtZGYx"
      "Yy00ZDc0LThiZGYtNmI1NzU5ZGNjMzlmIiwiaXNzIjoiaHR0cHM6Ly9hdXRoLnNiZXJjbGFz"
      "cy5ydS9hdXRoL3JlYWxtcy9FZHVQb3dlcktleWNsb2FrIiwiYXVkIjoiYWNjb3VudCIsInN1"
      "YiI6Ijc5NDQwZjA1LTJmOTUtNGI1MS04ZjA0LTEyZDExZTA3NjkwYSIsInR5cCI6IkJlYXJl"
      "ciIsImF6cCI6InMyMS1vcGVuLWFwaSIsInNlc3Npb25fc3RhdGUiOiJiOGJlZDU0YS0yN2E0"
      "LTQyMmEtYTc1OC05MjFlZDkzZjdkODIiLCJhY3IiOiIxIiwiYWxsb3dlZC1vcmlnaW5zIjpb"
      "Imh0dHBzOi8vZWR1LjIxLXNjaG9vbC5ydSJdLCJyZWFsbV9hY2Nlc3MiOnsicm9sZXMiOlsi"
      "b2ZmbGluZV9hY2Nlc3MiLCJ1bWFfYXV0aG9yaXphdGlvbiJdfSwicmVzb3VyY2VfYWNjZXNz"
      "Ijp7ImFjY291bnQiOnsicm9sZXMiOlsibWFuYWdlLWFjY291bnQiLCJtYW5hZ2UtYWNjb3Vu"
      "dC1saW5rcyIsInZpZXctcHJvZmlsZSJdfX0sInNjb3BlIjoicHJvZmlsZSBlbWFpbCIsImVt"
      "YWlsX3ZlcmlmaWVkIjp0cnVlLCJ1c2VyX2lkIjoiZDk2MzUzMmQtYzI0ZS00NTE0LWFjNzAt"
      "YzEyMzI5MGY3NmFiIiwibmFtZSI6Ik15ZXNoYSBTa2FhciIsImF1dGhfdHlwZV9jb2RlIjoi"
      "ZGVmYXVsdCIsInByZWZlcnJlZF91c2VybmFtZSI6Im15ZXNoYXNrQHN0dWRlbnQuMjEtc2No"
      "b29sLnJ1IiwiZ2l2ZW5fbmFtZSI6Ik15ZXNoYSIsImZhbWlseV9uYW1lIjoiU2thYXIiLCJl"
      "bWFpbCI6Im15ZXNoYXNrQHN0dWRlbnQuMjEtc2Nob29sLnJ1In0."
      "BTmLY5GRM98wSj1fbRAFPMiZ_"
      "82p5vKs9X96q13hen9VjJHGR2etgdtIRs2QeYBwTBgm5qN2gRqQ2vJ4QOh4Cc0rufCiVDxU5"
      "4DnLOGsqEIIDuMXU0nHKARD5kghSlAqGiVNuMciHtn6CaIBQuZtVv0golMYFsFqOTpZn4RBk"
      "I4HTJWMRByOuPRt6FFjBKIaGy4DnvteaDBXqHpxC2FqCZBywx60OWBTkAYxo-"
      "aTGPcaouxV8N2Hp_"
      "OnpjVcsrK5CWuL4boP2c2oMq6MwztyDU8PNwsmn5JWE9AZAXptqYc00dXUuRvm7LftnNISJ6"
      "gkBFmAGTtN9RMBpo8opn93WQ";
  std::string email_;
  std::string pass_;
};

#endif // SRC_S21_API_H
