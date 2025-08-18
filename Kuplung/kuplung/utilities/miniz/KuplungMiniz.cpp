//
//  KuplungMiniz.cpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "KuplungMiniz.hpp"
#include <iostream>
#include <fstream>

namespace KuplungApp::Utilities::Miniz {

KuplungMiniz::~KuplungMiniz(void) {
}

void KuplungMiniz::closeZipFile() {
  mz_zip_writer_finalize_archive(&this->zipFile);
  mz_zip_writer_end(&this->zipFile);
}

void KuplungMiniz::createZipFile(const std::string& zipFilename) {
  memset(&this->zipFile, 0, sizeof(mz_zip_archive));
  mz_zip_writer_init_file(&this->zipFile, zipFilename.c_str(), 0);
}

int KuplungMiniz::addFileToArchive(const std::string& contentPath, const std::string& zipPath) {
  if (!mz_zip_writer_add_file(&this->zipFile, zipPath.c_str(), contentPath.c_str(), "", 0, MZ_BEST_COMPRESSION)) {
    Settings::Instance()->funcDoLog("[Kuplung-Miniz-Add] failed!");
    return 1;
  }
  return 0;
}

bool KuplungMiniz::unzipArchive(std::string const& archiveFile, std::string const& archiveFolder) {
  std::string fullArchivePath = archiveFolder + "/" + archiveFile;
  std::ifstream zipFileStream(fullArchivePath, std::ios::binary | std::ios::in);
  if (zipFileStream) {
    zipFileStream.seekg(0, std::ios::end);
    std::streampos filesize = zipFileStream.tellg();
    zipFileStream.seekg(0, std::ios::beg);

    uint8_t* _buffer = new uint8_t[(uint32_t)filesize];
    zipFileStream.read((char*)_buffer, filesize);

    mz_zip_archive zipFile;
    memset(&zipFile, 0, sizeof(mz_zip_archive));
    if (mz_zip_reader_init_mem(&zipFile, _buffer, (size_t)filesize, 0) != 0) {
      for (int i = 0, len = (int)mz_zip_reader_get_num_files(&zipFile); i < len; i++) {
        mz_zip_archive_file_stat file_stat;
        if (!mz_zip_reader_file_stat(&zipFile, i, &file_stat))
          Settings::Instance()->funcDoLog("[Kuplung-Miniz-unzipArchive] mz_zip_reader_file_stat failed!");

        std::string unzippedFile = archiveFolder + "/" + std::string(file_stat.m_filename);
        if (!mz_zip_reader_extract_to_file(&zipFile, i, unzippedFile.c_str(), 0))
          Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[Kuplung-Miniz-unzipArchive] unzippedFile failed - ", unzippedFile.c_str(), "!"));
      }
    }
  }
  return true;


    /*
    mz_zip_archive src_archive;
    memset(&src_archive, 0, sizeof(src_archive));
    if (!mz_zip_reader_init_file(&src_archive, archiveFile.c_str(), 0)) {
        Settings::Instance()->funcDoLog(Settings::Instance()->string_format("[Kuplung-Miniz-unzipArchive] failed opening archive - ", archiveFile.c_str(), "!"));
        return false;
    }

    int i;
    for (i = mz_zip_reader_get_num_files(&src_archive) - 1; i >= 0; --i) {
        if (mz_zip_reader_is_file_encrypted(&src_archive, i))
            continue;

        mz_zip_archive_file_stat file_stat;
        bool status = mz_zip_reader_file_stat(&src_archive, i, &file_stat) != 0;

        int locate_file_index = mz_zip_reader_locate_file(&src_archive, file_stat.m_filename, NULL, 0);
        if (locate_file_index != i) {
            mz_zip_archive_file_stat locate_file_stat;
            mz_zip_reader_file_stat(&src_archive, locate_file_index, &locate_file_stat);
            if (_stricmp(locate_file_stat.m_filename, file_stat.m_filename) != 0)
                Settings::Instance()->funcDoLog("[Kuplung-Miniz-unzipArchive] mz_zip_reader_locate_file failed!");
            else
                Settings::Instance()->funcDoLog("[Kuplung-Miniz-unzipArchive] Warning: Duplicate filenames in archive!");
        }

        if ((file_stat.m_method) && (file_stat.m_method != MZ_DEFLATED))
            continue;

        if (status) {
            char name[260];
            mz_zip_reader_get_filename(&src_archive, i, name, sizeof(name));

            size_t extracted_size = 0;
            void *p = mz_zip_reader_extract_file_to_heap(&src_archive, name, &extracted_size, 0);
            if (!p)
                status = false;

            uint32 extracted_crc32 = MZ_CRC32_INIT;
            if (!mz_zip_reader_extract_file_to_callback(&src_archive, name, dummy_zip_file_write_callback, &extracted_crc32, 0))
                status = false;

            if (mz_crc32(MZ_CRC32_INIT, (const uint8*)p, extracted_size) != extracted_crc32)
                status = false;

            free(p);

            total_bytes_processed += file_stat.m_uncomp_size;
            total_files_processed++;
        }

        if (!status)
            break;
    }

    mz_zip_reader_end(&src_archive);

    return true;
    */
}

}
