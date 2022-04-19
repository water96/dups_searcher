#include "gtest/gtest.h"
#include "file.h"

const std::string TEST_DIR_PATH{ TEST_FILES_DIR };

TEST(File, DefaultConstructor)
{
    fl::File f;
    EXPECT_FALSE(f.IsOk());

    EXPECT_TRUE(f.GetFilePath().empty());
    EXPECT_TRUE(f.GetHashSum().empty());
    EXPECT_EQ(f.GetFileSize(), 0);
}

TEST(File, FileConstructor)
{
    const auto file = TEST_DIR_PATH + "/f1";

    fl::File f(file);
    EXPECT_TRUE(f.IsOk());
    EXPECT_NE(f.GetFileSize(), 0);
    EXPECT_EQ(f.GetFilePath(), file);
    EXPECT_FALSE(f.GetHashSum().empty());
}

TEST(File, DirConstructor)
{
    const auto file = TEST_DIR_PATH + "/d1";

    fl::File f(file);
    EXPECT_FALSE(f.IsOk());
    EXPECT_EQ(f.GetFilePath(), file);
    EXPECT_EQ(f.GetFileSize(), 0);
    EXPECT_TRUE(f.GetHashSum().empty());
}

TEST(File, EmptyFileConstructor)
{
    const auto file = TEST_DIR_PATH + "/empty_f";

    fl::File f(file);
    EXPECT_TRUE(f.IsOk());
    EXPECT_EQ(f.GetFilePath(), file);
    EXPECT_EQ(f.GetFileSize(), 0);
    EXPECT_FALSE(f.GetHashSum().empty());
}

TEST(File, LinkConstructor)
{
    const auto file = TEST_DIR_PATH + "/f1_link";

    fl::File f(file);
    EXPECT_TRUE(f.IsOk());
    EXPECT_EQ(f.GetFilePath(), file);
    EXPECT_NE(f.GetFileSize(), 0);
    EXPECT_FALSE(f.GetHashSum().empty());
}

TEST(File, CopyConstructor)
{
    const auto file = TEST_DIR_PATH + "/f1";

    fl::File f(file);
    fl::File f2(f);

    EXPECT_EQ(f.IsOk(), f2.IsOk());
    EXPECT_EQ(f.GetFilePath(), f2.GetFilePath());
    EXPECT_EQ(f.GetFileSize(), f2.GetFileSize());
    EXPECT_EQ(f.GetHashSum(), f2.GetHashSum());
}

TEST(File, MoveConstructor)
{
    const auto file = TEST_DIR_PATH + "/f1";

    fl::File f(file);
    fl::File f2(std::move(f));

    //for new f2
    EXPECT_TRUE(f2.IsOk());
    EXPECT_NE(f2.GetFileSize(), 0);
    EXPECT_EQ(f2.GetFilePath(), file);
    EXPECT_FALSE(f2.GetHashSum().empty());

    //for old f
    EXPECT_FALSE(f.IsOk());
    EXPECT_EQ(f.GetFileSize(), 0);
    EXPECT_TRUE(f.GetFilePath().empty());
    EXPECT_TRUE(f.GetHashSum().empty());
}

TEST(File, ComparisonFilesTheSame)
{
    const auto file1 = TEST_DIR_PATH + "/f1";
    const auto file2 = TEST_DIR_PATH + "/f1";

    fl::File f(file1);
    fl::File f2(file2);

    EXPECT_EQ(f, f2);
}

TEST(File, ComparisonFiles)
{
    const auto file1 = TEST_DIR_PATH + "/f1";
    const auto file2 = TEST_DIR_PATH + "/f2";

    fl::File f(file1);
    fl::File f2(file2);

    EXPECT_EQ(f, f2);
}

TEST(File, ComparisonFilesDiff)
{
    const auto file1 = TEST_DIR_PATH + "/f1";
    const auto file2 = TEST_DIR_PATH + "/another_f";

    fl::File f(file1);
    fl::File f2(file2);

    EXPECT_FALSE(f == f2);
}

TEST(File, ComparisonFilesLink)
{
    const auto file1 = TEST_DIR_PATH + "/f1";
    const auto file2 = TEST_DIR_PATH + "/f1_link";

    fl::File f(file1);
    fl::File f2(file2);

    EXPECT_TRUE(f == f2);
}

TEST(File, ComparisonDirs)
{
    const auto file1 = TEST_DIR_PATH + "/d1";
    const auto file2 = TEST_DIR_PATH + "/d2";

    fl::File f(file1);
    fl::File f2(file2);

    EXPECT_FALSE(f == f2);
}

TEST(File, ComparisonDirAndFile)
{
    const auto file1 = TEST_DIR_PATH + "/d1";
    const auto file2 = TEST_DIR_PATH + "/f2";

    fl::File f(file1);
    fl::File f2(file2);

    EXPECT_FALSE(f == f2);
}

TEST(File, ComparisonDirAndFileInDir)
{
    const auto file1 = TEST_DIR_PATH + "/d1/f1";
    const auto file2 = TEST_DIR_PATH + "/f1";

    fl::File f(file1);
    fl::File f2(file2);

    EXPECT_TRUE(f == f2);
}


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
