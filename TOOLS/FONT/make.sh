#
#
#

HEIGHT=64
FONT=$1	#"arial.ttf"
OUT=$2 #"fntLA.png"



#==============================================================================
echo Generating text stencils...
HH=$(( $HEIGHT / 2 ))

convert			\
    -background black	\
    -fill white		\
    -font $FONT		\
    -size x$HEIGHT	\
    label:@chars	\
    -gravity center	\
    __stencil_ft.png

convert				\
    -background black		\
    -fill white			\
    -size $HEIGHT"x"$HEIGHT	\
    xc:black -draw "circle $HH,$HH $HH,$(( $HH + $HH / 3 ))" \
    __stencil_fb.png


montage -label ''	\
    -geometry +0+0	\
    __stencil_ft.png	\
    __stencil_fb.png	\
    __stencil_f.png





convert			\
    -background black	\
    -fill black		\
    -stroke white	\
    -strokewidth 1	\
    -font $FONT		\
    -size x$HEIGHT	\
    label:@chars	\
    -gravity center	\
    __stencil_st.png

convert			\
    -background black	\
    -fill black		\
    -stroke white	\
    -strokewidth 1	\
    -size $HEIGHT"x"$HEIGHT				\
    xc:black -draw "circle $HH,$HH $HH,$(( $HH / 6 ))"	\
    __stencil_sb.png


montage -label ''	\
    -geometry +0+0	\
    __stencil_st.png	\
    __stencil_sb.png	\
    __stencil_s.png



#==============================================================================
echo Retrieving size...
IMGSIZE=$(identify __stencil_f.png | awk '{print $3}')
IMGW=$(echo $IMGSIZE | awk -Fx '{print $1}')
IMGH=$(echo $IMGSIZE | awk -Fx '{print $2}')



#==============================================================================
echo Generating background...
convert				\
    -size $IMGH"x"$IMGH		\
    -seed 0			\
    plasma:gray-white		\
    -paint 2			\
    -swirl 300			\
    -colorspace Gray		\
    __background_f.png

convert				\
    -size $IMGH"x"$IMGH		\
    -seed 1			\
    plasma:white-white		\
    -paint 2			\
    -swirl 300			\
    -colorspace Gray		\
    __background_s.png



#==============================================================================
echo Masking...
convert				\
    -size $IMGW"x"$IMGH		\
    tile:__background_f.png	\
    __stencil_f.png		\
    -alpha Off			\
    -compose Copy_Opacity	\
    -composite __text_f.png

convert				\
    -size $IMGW"x"$IMGH		\
    tile:__background_s.png	\
    __stencil_s.png		\
    -alpha Off			\
    -compose Copy_Opacity	\
    -composite __text_s.png



#==============================================================================
echo Assembling...
composite -compose ATop __text_s.png __text_f.png __assembled.png




#==============================================================================
echo Crunching...
cd fntcrunch
make
cd ..
./fntc __assembled.png


rm fntc __*.png
mv out.png $OUT
echo done

#EoF===========================================================================
