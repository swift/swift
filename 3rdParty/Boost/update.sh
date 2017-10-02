#!/usr/bin/env bash
set -e
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
	algorithm/string.hpp \
	archive/text_iarchive.hpp \
	archive/text_oarchive.hpp \
	asio.hpp \
	assign/list_of.hpp \
	bind.hpp \
	cast.hpp \
	date_time/posix_time/posix_time.hpp \
	date_time/posix_time/time_serialize.hpp \
	date_time/local_time/local_time.hpp \
	date_time/c_local_time_adjustor.hpp \
	date_time/gregorian/gregorian_types.hpp \
	filesystem.hpp \
	filesystem/fstream.hpp \
	format.hpp \
	lambda/lambda.hpp \
	lambda/bind.hpp \
	logic/tribool.hpp \
	multi_index_container.hpp \
	multi_index/hashed_index.hpp \
	multi_index/member.hpp \
	multi_index/sequenced_index.hpp \
	noncopyable.hpp \
	numeric/conversion/cast.hpp \
	optional.hpp \
	program_options.hpp \
	random/mersenne_twister.hpp \
	random/variate_generator.hpp \
	random/uniform_int.hpp \
	regex.hpp \
	scope_exit.hpp \
	serialization/serialization.hpp \
	serialization/vector.hpp \
	serialization/list.hpp \
	serialization/map.hpp \
	serialization/shared_ptr.hpp \
	serialization/string.hpp \
	serialization/set.hpp \
	serialization/version.hpp \
	serialization/split_member.hpp \
	serialization/optional.hpp \
	signals2.hpp \
	thread.hpp \
	unordered_map.hpp \
	uuid/uuid.hpp \
	uuid/uuid_io.hpp \
	uuid/uuid_generators.hpp \
	variant.hpp \
	spirit/include/lex_lexertl.hpp \
	typeof/incr_registration_group.hpp \
	atomic \
	system/src/error_code.cpp \
	phoenix/support/detail/iterate.hpp \
	type_traits.hpp \
	range/adaptor/reversed.hpp \
	$TARGET_DIR
cp $1/LICENSE_1_0.txt $TARGET_DIR

rm -rf $TARGET_DIR/libs/config
rm -rf $TARGET_DIR/libs/smart_ptr

LIBS="date_time regex system thread signals2 filesystem multi_index program_options serialization archive atomic"
for lib in $LIBS; do
	rm -rf $TARGET_DIR/libs/$lib/build $TARGET_DIR/libs/$lib/*.doc $TARGET_DIR/libs/$lib/src/*.doc $TARGET_DIR/libs/$lib/src/CMakeLists.txt $TARGET_DIR/libs/$lib/test
done
rm -rf $TARGET_DIR/tools/bcp/*.html $TARGET_DIR/libs/test $TARGET_DIR/doc $TARGET_DIR/boost.png $TARGET_DIR/boost/test $TARGET_DIR/tools/bcp/Jamfile.v2 $TARGET_DIR/tools/bcp/doc $TARGET_DIR/tools/bcp/test $TARGET_DIR/Jamroot

mv $TARGET_DIR/libs/filesystem/src/utf8_codecvt_facet.cpp $TARGET_DIR/libs/filesystem/src/filesystem_utf8_codecvt_facet.cpp 
mv $TARGET_DIR/libs/program_options/src/utf8_codecvt_facet.cpp $TARGET_DIR/libs/program_options/src/program_options_utf8_codecvt_facet.cpp

for diff in *.diff; do
	patch -p3 < $diff
done
