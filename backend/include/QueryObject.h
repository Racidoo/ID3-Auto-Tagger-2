#if !defined(QUERY_OBJECT_H)
#define QUERY_OBJECT_H

#include <string>

class QueryObject {
  protected:
    const std::string id;
    const std::string name;
    const std::string type;
    const std::string imageURL;

  public:
    QueryObject(const std::string &_id, const std::string &_name,
                const std::string &_type, const std::string &_imageURL);
    ~QueryObject();

    inline const std::string &get_id() const { return id; }
    inline const std::string &get_name() const { return name; }
    inline const std::string &get_type() const { return type; }
    inline const std::string get_imageUrl() const { return imageURL; }
};

#endif // QUERY_OBJECT_H
