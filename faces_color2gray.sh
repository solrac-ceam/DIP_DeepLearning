rm -f faces_gray/*
mkdir -p faces_gray
imgName=
pathImgSource=
pathImgtarget=
for img in $(ls faces_pubfigs/ | grep ".jpg"); do
    imgName=`echo $img | sed -e "s/.jpg//"`
    pathImgSource=`echo "faces_pubfigs/"$img`
    pathImgtarget=`echo "faces_gray/"$imgName".pgm"`
    echo "Convert "$pathImgSource" to "$pathImgtarget
    convert $pathImgSource $pathImgtarget
done