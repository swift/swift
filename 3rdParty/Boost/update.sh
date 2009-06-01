if [ -z "$1" ]; then
	echo "Please specify the location of the boost source tree"
	exit -1
fi

bcp --boost="$1" \
	bind.hpp \
	date_time/posix_time/posix_time.hpp \
	foreach.hpp \
	filesystem.hpp \
	filesystem/fstream.hpp \
	noncopyable.hpp \
	numeric/conversion/cast.hpp \
	shared_ptr.hpp \
  optional.hpp \
	signals.hpp \
	thread.hpp asio.hpp \
	.

rm -rf libs/config
rm -rf libs/smart_ptr

LIBS="date_time regex system thread signals filesystem"
for lib in $LIBS; do
	rm -rf libs/$lib/build libs/$lib/*.doc libs/$lib/src/*.doc libs/$lib/src/CMakeLists.txt libs/$lib/test
done

# We don't use regex (yet)
rm -rf libs/regex
