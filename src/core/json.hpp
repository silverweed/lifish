#define PICOJSON_USE_INT64
#define PICOJSON_USE_LOCALE 0
#include <picojson.h>

template <typename T>
inline T get_or(const picojson::object &obj, const std::string &key, T dflt = {}) {
  auto it = obj.find(key);
  if (it == obj.end())
    return dflt;
  return it->second.get<T>();
}
