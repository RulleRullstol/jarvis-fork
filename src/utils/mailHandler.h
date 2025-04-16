

#include <string>

void sendManuel();
void sendMail(const std::string &rec, const std::string &subj,
              const std::string &body, std::string &pdfPath);
