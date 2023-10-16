#include <papyrus/PapyrusScript.h>

#include <common/CapricaConfig.h>
#include <common/EngineLimits.h>
#include <common/OSUtils.h>

namespace caprica { namespace papyrus {

pex::PexFile* PapyrusScript::buildPex(CapricaReportingContext& repCtx) const {
  auto alloc = new allocators::ChainedPool(1024 * 4);
  auto pex = alloc->make<pex::PexFile>(alloc);
  pex->setGameAndVersion(conf::Papyrus::game);
  if (conf::CodeGeneration::emitDebugInfo) {
    pex->debugInfo = alloc->make<pex::PexDebugInfo>();
    pex->debugInfo->modificationTime = lastModificationTime;
  }
  pex->compilationTime = time(nullptr);
  pex->sourceFileName = pex->alloc->allocateString(sourceFileName);

  static std::string computerName = getComputerName();
  pex->computerName = computerName;

  static std::string userName = getUserName();
  pex->userName = userName;

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
