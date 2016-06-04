#include <papyrus/PapyrusNamespaceResolutionContext.h>

#include <common/CapricaReportingContext.h>
#include <common/Concurrency.h>

namespace caprica { namespace papyrus {

namespace {

struct PapyrusNamespace final
{
  std::string fullName{ "" };
  std::string name{ "" };
  PapyrusNamespace* parent{ nullptr };
  caseless_concurrent_unordered_identifier_map<std::string, PapyrusNamespace*> children{ };
  // Key is unqualified name, value is full path to file.
  caseless_unordered_identifier_map<std::string> objects{ };

  void createNamespace(boost::string_ref curPiece, caseless_unordered_identifier_map<std::string>&& map) {
    if (curPiece == "") {
      objects = std::move(map);
      return;
    }

    boost::string_ref curSearchPiece = curPiece;
    boost::string_ref nextSearchPiece = "";
    auto loc = curPiece.find(':');
    if (loc != boost::string_ref::npos) {
      curSearchPiece = curPiece.substr(0, loc);
      nextSearchPiece = curPiece.substr(loc + 1);
    }

    auto curSearchStr = curSearchPiece.to_string();
    auto f = children.find(curSearchStr);
    if (f == children.end()) {
      auto n = new PapyrusNamespace();
      n->name = curSearchStr;
      n->fullName = curSearchStr;
      if (fullName != "")
        n->fullName = fullName + ':' + curSearchStr;
      n->parent = this;
      children.insert({ curSearchStr, n });
      f = children.find(curSearchStr);
    }
    f->second->createNamespace(nextSearchPiece, std::move(map));
  }

  bool tryFindNamespace(boost::string_ref curPiece, PapyrusNamespace const** ret) const {
    if (curPiece == "") {
      *ret = this;
      return true;
    }

    boost::string_ref curSearchPiece = curPiece;
    boost::string_ref nextSearchPiece = "";
    auto loc = curPiece.find(':');
    if (loc != boost::string_ref::npos) {
      curSearchPiece = curPiece.substr(0, loc);
      nextSearchPiece = curPiece.substr(loc + 1);
    }

    auto f = children.find(curSearchPiece.to_string());
    if (f != children.end())
      return f->second->tryFindNamespace(nextSearchPiece, ret);

    return false;
  }

  bool tryFindType(boost::string_ref typeName, std::string* retTypeName, std::string* retTypePath, std::string* retStructName) const {
    auto loc = typeName.find(':');
    if (loc == boost::string_ref::npos) {
      auto tnStr = typeName.to_string();
      if (objects.find(tnStr) != objects.end()) {
        if (fullName != "")
          *retTypeName = fullName + ':' + tnStr;
        else
          *retTypeName = tnStr;
        *retTypePath = objects.at(tnStr);
        return true;
      }
      return false;
    }

    // It's a partially qualified type name, or else is referencing
    // a struct.
    auto baseName = typeName.substr(0, loc).to_string();
    auto subName = typeName.substr(loc + 1);

    // It's a partially qualified name.
    auto f = children.find(baseName);
    if (f != children.end())
      return f->second->tryFindType(subName, retTypeName, retTypePath, retStructName);

    // subName is still partially qualified, so it can't
    // be referencing a struct in this namespace.
    if (subName.find(':') != boost::string_ref::npos)
      return false;

    // It is a struct reference.
    if (objects.find(baseName) != objects.end()) {
      if (fullName != "")
        *retTypeName = fullName + ':' + baseName;
      else
        *retTypeName = baseName;
      *retTypePath = objects.at(baseName);
      *retStructName = subName.to_string();
      return true;
    }

    return false;
  }
};
}

static PapyrusNamespace rootNamespace{ };
void PapyrusNamespaceResolutionContext::pushNamespaceFullContents(const std::string& namespaceName, caseless_unordered_identifier_map<std::string>&& map) {
  rootNamespace.createNamespace(namespaceName, std::move(map));
}

bool PapyrusNamespaceResolutionContext::tryFindType(const std::string& baseNamespace,
                                                    const std::string& typeName,
                                                    std::string* retFullTypeName,
                                                    std::string* retFullTypePath,
                                                    std::string* retStructName) {
  const PapyrusNamespace* curNamespace = nullptr;
  if (!rootNamespace.tryFindNamespace(baseNamespace, &curNamespace))
    return false;

  while (curNamespace != nullptr) {
    if (curNamespace->tryFindType(typeName, retFullTypeName, retFullTypePath, retStructName))
      return true;
    curNamespace = curNamespace->parent;
  }
  return false;
}

}}
