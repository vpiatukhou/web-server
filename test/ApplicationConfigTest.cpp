#include "ApplicationConfig.h"
#include "Resources.h"

#include "gtest/gtest.h"

#include <filesystem>

namespace {
    using namespace WebServer;
    using namespace std::string_literals;

    const std::string BASE_DIR = BINARY_DIR + "/resources/"s;
    const std::string EMPTY_APPLICATION_CONFIG_FILEPATH = BASE_DIR + "emptyApplication.json"s;
    const std::string CONFIG_WITH_BASE_DIR_WITHOUT_LAST_DELIMITER = BASE_DIR + "configWithBaseDirWithoutLastDelimiter.json"s;
    const std::string CONFIG_WITH_ABSOLUTE_NOT_FOUND_PAGE_PATH = BASE_DIR + "configWithAbsoluteNotFoundPagePath.json"s;
    const std::string APPLICATION_CONFIG_FILEPATH = BASE_DIR + "application.json"s;

    void verifyDefaultValues(const ApplicationConfig& config) {
        ASSERT_EQ(8080, config.getServerPort());
        ASSERT_EQ("Web Server", config.getServerName());
        ASSERT_EQ(false, config.isSslEnabled());
        ASSERT_EQ("", config.getSslCertificatePath());
        ASSERT_EQ("", config.getSslPrivateKeyPath());
        ASSERT_EQ("", config.getSslPassword());
        ASSERT_EQ("", config.getSslDhFilepath());
        ASSERT_EQ("/var/www/webserver/", config.getStaticResouceBaseDir());
        ASSERT_EQ("/var/www/webserver/pages/notFound.html", config.getNotFoundPage());
    }
}

TEST(ApplicationConfigTest, noConfigFile) {
    //when
    ApplicationConfig config("");

    //then
    verifyDefaultValues(config);
}

TEST(ApplicationConfigTest, emptyConfigFile) {
    //given
    ASSERT_TRUE(std::filesystem::is_regular_file(EMPTY_APPLICATION_CONFIG_FILEPATH)) 
        << "Invalid test data. The file '" << EMPTY_APPLICATION_CONFIG_FILEPATH << "' was not found.";

    //when
    ApplicationConfig config(EMPTY_APPLICATION_CONFIG_FILEPATH);

    //then
    verifyDefaultValues(config);
}

TEST(ApplicationConfigTest, readAllProperties) {
    //when
    ApplicationConfig config(APPLICATION_CONFIG_FILEPATH);

    //then
    ASSERT_EQ(8081, config.getServerPort());
    ASSERT_EQ("aaa", config.getServerName());
    ASSERT_EQ(true, config.isSslEnabled());
    ASSERT_EQ("/ssl/cert.pem", config.getSslCertificatePath());
    ASSERT_EQ("/ssl/certkey.key", config.getSslPrivateKeyPath());
    ASSERT_EQ("aaa123", config.getSslPassword());
    ASSERT_EQ("/ssl/aaa.pem", config.getSslDhFilepath());
    ASSERT_EQ("/base/dir/", config.getStaticResouceBaseDir());
    ASSERT_EQ("/base/dir/error/aaa.html", config.getNotFoundPage());
    ASSERT_EQ(2, config.getMediaTypeMapping().size());
    auto& mapping1 = config.getMediaTypeMapping()[0];
    auto& mapping2 = config.getMediaTypeMapping()[1];
    ASSERT_EQ(".+[.]json$", mapping1.filenameRegExp);
    ASSERT_EQ("application/json", mapping1.mediaType);
    ASSERT_EQ(".+[.]tiff$", mapping2.filenameRegExp);
    ASSERT_EQ("image/tiff", mapping2.mediaType);
}

TEST(ApplicationConfigTest, lastDelimiterIsAppendedToBaseDir) {
    //when
    ApplicationConfig config(CONFIG_WITH_BASE_DIR_WITHOUT_LAST_DELIMITER);

    //then
    //verify that the last delimiter was appended
    ASSERT_EQ("/base/dir/", config.getStaticResouceBaseDir());
    //verify that there is no duplicate delimiter
    ASSERT_EQ("/base/dir/error/aaa.html", config.getNotFoundPage());
}

TEST(ApplicationConfigTest, makeNotFoundPagePathRelative) {
    //when
    ApplicationConfig config(CONFIG_WITH_ABSOLUTE_NOT_FOUND_PAGE_PATH);

    //then
    //verify that the base path and notFound path were correctly concatenated
    ASSERT_EQ("/base/dir/error/aaa.html", config.getNotFoundPage());
}
