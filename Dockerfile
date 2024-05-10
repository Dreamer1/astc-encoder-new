FROM ubuntu:22.04
ENV PKG_CONFIG_PATH="/usr/local/lib/pkgconfig"
## mount dir


RUN  apt update && \
     apt-get -y install  git  wget make cmake unzip cron vim libaom3 libaom-dev libwebp7 nasm x265 zstd libzip4 build-essential libtiff-dev libpng-dev libmagickwand-dev zlib1g-dev libfreetype-dev autoconf automake autotools-dev libtool pkg-config libstdc++-11-dev \
  libde265-dev \
  libdjvulibre-dev \
  libfftw3-dev \
  libghc-bzlib-dev \
  libgoogle-perftools-dev \
  libgraphviz-dev \
  libgs-dev \
  libheif-dev \
  libjbig-dev \
  libjemalloc-dev \
  liblcms2-dev \
  liblqr-1-0-dev \
  liblzma-dev \
  libopenexr-dev \
  libopenjp2-7-dev \
  libpango1.0-dev \
  libraqm-dev \
  libraw-dev \
  librsvg2-dev \
  libtiff-dev \
  libwebp-dev \
  libwmf-dev \
  libxml2-dev \
  libzip-dev \
  libzstd-dev && \
ldconfig /usr/local/lib

 # install astc-encoder
RUN  git clone https://github.com/Dreamer1/astc-encoder-new.git && \
    cd astc-encoder-new/ && \
mkdir build && \
cd build&& \
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../   \
-DASTCENC_ISA_AVX2=ON -DASTCENC_ISA_SSE41=ON -DASTCENC_ISA_SSE2=ON -DASTCENC_SHAREDLIB=ON .. && \
make install -j 4 && \
ln -s /astc-encoder-new/lib/* /usr/local/lib  &&\
ln -s /astc-encoder-new/lib/* /lib/x86_64-linux-gnu/ && \
     cp /astc-encoder-new/bin/astcenc-avx2 /usr/local/bin/ && \
     cp /astc-encoder-new/bin/astcenc-sse4.1 /usr/local/bin/ && \
     cp /astc-encoder-new/bin/astcenc-sse2 /usr/local/bin/ && \
     ln -s /usr/local/bin/astcenc-avx2 /usr/local/bin/astcenc  && \
      ldconfig /usr/local/lib

 # install go
RUN wget https://dl.google.com/go/go1.20.4.linux-amd64.tar.gz && \
tar -C /usr/local -xzf go1.20.4.linux-amd64.tar.gz && \
rm go1.20.4.linux-amd64.tar.gz
ENV PATH=$PATH:/usr/local/go/bin


 # install ImageMagick
RUN git clone --branch 7.1.1-31 --single-branch https://github.com/ImageMagick/ImageMagick.git && \
cd ImageMagick && \
   ./configure \
      --with-bzlib=yes \
  --with-djvu=yes \
  --with-dps=yes \
  --with-fftw=yes \
  --with-flif=yes \
  --with-fontconfig=yes \
  --with-fpx=yes \
  --with-freetype=yes \
  --with-gslib=yes \
  --with-gvc=yes \
  --with-heic=yes \
  --with-jbig=yes \
  --with-jemalloc=yes \
  --with-jpeg=yes \
  --with-jxl=yes \
  --with-lcms=yes \
  --with-lqr=yes \
  --with-lzma=yes \
  --with-magick-plus-plus=yes \
  --with-openexr=yes \
  --with-openjp2=yes \
  --with-pango=yes \
  --with-perl=yes \
  --with-png=yes \
  --with-raqm=yes \
  --with-raw=yes \
  --with-rsvg=yes \
  --with-tcmalloc=yes \
  --with-tiff=yes \
  --with-webp=yes \
  --with-wmf=yes \
  --with-x=yes \
  --with-xml=yes \
  --with-zip=yes \
  --with-zlib=yes \
  --with-zstd=yes \
  --with-gcc-arch=native \
&& make -j$(nproc) && make install && \
     ldconfig /usr/local/lib

ENV LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH

WORKDIR /home/services