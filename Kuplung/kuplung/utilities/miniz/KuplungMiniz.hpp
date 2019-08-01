//
//  KuplungMiniz.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#ifndef KuplungMiniz_hpp
#define KuplungMiniz_hpp

#include "kuplung/settings/Settings.h"
#include "miniz.h"

namespace KuplungApp { namespace Utilities { namespace Miniz {

class KuplungMiniz {
public:
  ~KuplungMiniz(void);

  void createZipFile(const std::string& zipFilename);
  int addFileToArchive(const std::string& contentPath, const std::string& zipPath="");
  bool unzipArchive(std::string const& archiveFile, std::string const& archiveFolder);
  void closeZipFile();

private:
  mz_zip_archive zipFile;
};

}}}

#endif /* KuplungMiniz_hpp */
