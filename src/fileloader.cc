#include "fileloader.h"

// Standard C++
#include <string>

// Standard C
// Unfortunally these are needed as long as we don't use a third-party C++ library or similar.
// There simply is no way to grab last midified date time with the C++ headers.

bool CFileLoader::loadFile(const std::string& filename, bool useCache)
{
    return true;
}
