if [ -z "$1" ]; then
	echo "Please specify the location of the boost source tree"
	exit -1
fi

TARGET_DIR=src

if [ ! -d "$TARGET_DIR" ]; then
	mkdir $TARGET_DIR
fi

./bcp --boost="$1" \
	tools/bcp \
	bind.hpp \
	cast.hpp \
	date_time/posix_time/posix_time.hpp \
	date_time/local_time/local_time.hpp \
	foreach.hpp \
	filesystem.hpp \
	filesystem/fstream.hpp \
	noncopyable.hpp \
	numeric/conversion/cast.hpp \
	shared_ptr.hpp \
	smart_ptr/make_shared.hpp \
	optional.hpp \
	signals.hpp \
	program_options.hpp \
	thread.hpp \
	asio.hpp \
	uuid/uuid.hpp \
	uuid/uuid_io.hpp \
	uuid/uuid_generators.hpp \
	variant.hpp \
	regex.hpp \
	unordered_map.hpp \
	algorithm/string.hpp \
	format.hpp \
	logic/tribool.hpp \
	assign/list_of.hpp \
	$TARGET_DIR

rm -rf $TARGET_DIR/libs/config
rm -rf $TARGET_DIR/libs/smart_ptr

LIBS="date_time regex system thread signals filesystem program_options"
for lib in $LIBS; do
	rm -rf $TARGET_DIR/libs/$lib/build $TARGET_DIR/libs/$lib/*.doc $TARGET_DIR/libs/$lib/src/*.doc $TARGET_DIR/libs/$lib/src/CMakeLists.txt $TARGET_DIR/libs/$lib/test
done
rm -rf $TARGET_DIR/tools/bcp/*.html $TARGET_DIR/libs/test $TARGET_DIR/doc $TARGET_DIR/boost.png $TARGET_DIR/boost/test $TARGET_DIR/tools/bcp/Jamfile.v2 $TARGET_DIR/tools/bcp/doc $TARGET_DIR/tools/bcp/test $TARGET_DIR/Jamroot
rm -rf $TARGET_DIR/libs/filesystem/v2/build $TARGET_DIR/libs/filesystem/v3/build

mv $TARGET_DIR/libs/filesystem/v3/src/utf8_codecvt_facet.cpp $TARGET_DIR/libs/filesystem/v3/src/filesystem_utf8_codecvt_facet.cpp 
mv $TARGET_DIR/libs/program_options/src/utf8_codecvt_facet.cpp $TARGET_DIR/libs/program_options/src/program_options_utf8_codecvt_facet.cpp

for diff in *.diff; do
	patch -p3 < $diff
done
