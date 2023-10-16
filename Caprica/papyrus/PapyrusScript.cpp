#include <papyrus/PapyrusScript.h>

#include <common/CapricaConfig.h>
#include <common/EngineLimits.h>
#include <common/OSUtils.h>

namespace caprica { namespace papyrus {
static constexpr const char *USERNAME_PLACEHOLDER = "<USERNAME>";
pex::PexFile* PapyrusScript::buildPex(CapricaReportingContext& repCtx) const {
  auto alloc = new allocators::ChainedPool(1024 * 4);
  auto pex = alloc->make<pex::PexFile>(alloc);
  pex->setGameAndVersion(conf::Papyrus::game);
  if (conf::CodeGeneration::emitDebugInfo) {
    pex->debugInfo = alloc->make<pex::PexDebugInfo>();
    pex->debugInfo->modificationTime = lastModificationTime;
  }
  pex->compilationTime = time(nullptr);
  std::filesystem::path file = this->sourceFileName;
  // Anonymizing path names
  // we are doing this for the ease of debugging; we need to know the full pathname of the file in the debugger,
  // but we don't want to leak the username. So, we just replace the username with a constant string.
  // pathnames that contain usernames start with one of the following:
  // - C:\Users\<USERNAME>
  // - /home/<USERNAME>
  // - /Users/<USERNAME>
  if (conf::General::anonymizeOutput && file.is_absolute()) {
    auto rel = file.relative_path();
    auto firstDirIter = rel.begin();
    auto firstDir = *firstDirIter;
    if (firstDir == "Users" || firstDir == "home") {
      auto end = rel.end();
      // construct a new path with the username placeholder
      auto newPath = file.root_path() / firstDir / std::filesystem::path(USERNAME_PLACEHOLDER);
      // get the rest of the path after firstDir
      for (auto it = (++(++firstDirIter)); it != end; ++it) {
        newPath /= *it;
      }
      file = newPath;
    }
  }

  pex->sourceFileName = pex->alloc->allocateString(file.string());

  static std::string computerName = getComputerName();
  pex->computerName = conf::General::anonymizeOutput ? "" : computerName;

  static std::string userName = getUserName();
  pex->userName = conf::General::anonymizeOutput ? "" : userName;




  for (auto o : objects)
    o->buildPex(repCtx, pex);

  if (objects.size()) {
    EngineLimits::checkLimit(repCtx,
                             objects.front()->location,
                             EngineLimits::Type::PexFile_UserFlagCount,
                             pex->getUserFlagCount());
  }
  return pex;
}

}}
