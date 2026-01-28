#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>
#include <vector>

// Include các class cần test
#include "service/scanner/IFileSystem.h"
#include "service/scanner/FileScanner.h"

// Sử dụng namespace của Google Test/Mock cho gọn
using ::testing::Return;
using ::testing::_;
using ::testing::NiceMock;

// 1. TẠO CLASS GIẢ LẬP (MOCK)
// Thay vì đọc ổ cứng thật, class này sẽ trả về dữ liệu giả do ta quy định
class MockFileSystem : public IFileSystem {
public:
    MOCK_METHOD(bool, exists, (const std::string& path), (const, override));
    MOCK_METHOD(bool, isDirectory, (const std::string& path), (const, override));
    MOCK_METHOD(std::vector<std::string>, listEntries, (const std::string& path), (const, override));
};

// 2. VIẾT TEST CASE
// Test trường hợp: Quét thư mục phẳng (không đệ quy) chứa file hỗn hợp
TEST(FileScannerTest, ScanFlatDirectory_ShouldFilterMediaFiles) {
    // --- ARRANGE (Chuẩn bị) ---
    // Tạo Mock Object
    auto mockFS = std::make_shared<NiceMock<MockFileSystem>>();
    
    // Inject Mock vào Scanner
    FileScanner scanner(mockFS);

    std::string rootPath = "/fake/music";

    // Setup hành vi cho Mock (Dạy cho nó cách nói dối)
    // 1. Giả vờ rằng đường dẫn rootPath có tồn tại
    EXPECT_CALL(*mockFS, exists(rootPath)).WillOnce(Return(true));

    // 2. Giả vờ rằng trong thư mục đó có 3 file
    std::vector<std::string> fakeEntries = {
        "/fake/music/song1.mp3",   // Should keep (Audio)
        "/fake/music/readme.txt",  // Should ignore
        "/fake/music/movie.mp4"    // Should keep (Video)
    };
    EXPECT_CALL(*mockFS, listEntries(rootPath)).WillOnce(Return(fakeEntries));

    // 3. Giả vờ rằng tất cả các entry trên đều là File (không phải thư mục con)
    EXPECT_CALL(*mockFS, isDirectory("/fake/music/song1.mp3")).WillRepeatedly(Return(false));
    EXPECT_CALL(*mockFS, isDirectory("/fake/music/readme.txt")).WillRepeatedly(Return(false));
    EXPECT_CALL(*mockFS, isDirectory("/fake/music/movie.mp4")).WillRepeatedly(Return(false));

    // --- ACT (Thực hiện hành động) ---
    std::vector<MediaFile> results = scanner.scanDirectory(rootPath);

    // --- ASSERT (Kiểm tra kết quả) ---
    // Mong đợi tìm thấy 2 file (mp3 và mp4), bỏ qua txt
    ASSERT_EQ(results.size(), 2);

    // Kiểm tra chi tiết file đầu tiên
    EXPECT_EQ(results[0].getFileName(), "song1.mp3");
    EXPECT_EQ(results[0].getType(), MediaType::Audio);

    // Kiểm tra chi tiết file thứ hai
    EXPECT_EQ(results[1].getFileName(), "movie.mp4");
    EXPECT_EQ(results[1].getType(), MediaType::Video);
}