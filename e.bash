SANITIZER_FLAGS="-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr"
INCLUDES="-Imy_libs/"
LINUX_FLAGS="-O3 -D_STK_LEVEL_OF_CHECK=2 -DDEBUG -D_STK_FLT_TYPE"
g++ stack.cpp MyLibs/helper_funcs.cpp stackmain.cpp $SANITIZER_FLAGS $INCLUDES $LINUX_FLAGS -o bin/stack
