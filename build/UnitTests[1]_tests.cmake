add_test([=[FileScannerTest.ScanFlatDirectory_ShouldFilterMediaFiles]=]  /home/duong/MediaPlayerProject/build/bin/UnitTests [==[--gtest_filter=FileScannerTest.ScanFlatDirectory_ShouldFilterMediaFiles]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[FileScannerTest.ScanFlatDirectory_ShouldFilterMediaFiles]=]  PROPERTIES WORKING_DIRECTORY /home/duong/MediaPlayerProject/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set(  UnitTests_TESTS FileScannerTest.ScanFlatDirectory_ShouldFilterMediaFiles)
