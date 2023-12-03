# oceanus

后台 C++ 接口，对券商提供的行情和交易的标准封装


## centos 编译环境

### 安装 gcc 9
``shell
yum install centos-release-scl-rh -y
yum install devtoolset-9-build -y
yum install devtoolset-9-gdb* -y
yum install devtoolset-9-gcc devtoolset-9-gcc-c++ -y
source /opt/rh/devtoolset-9/enable
``

### 安装 redis 7 
``shell
yum install -y http://rpms.famillecollet.com/enterprise/remi-release-7.rpm
yum --enablerepo=remi install redis
``

### 安装 Apache Arrow
``shell
sudo yum install -y epel-release || sudo yum install -y https://dl.fedoraproject.org/pub/epel/epel-release-latest-$(cut -d: -f5 /etc/system-release-cpe | cut -d. -f1).noarch.rpm
sudo yum install -y https://apache.jfrog.io/artifactory/arrow/centos/$(cut -d: -f5 /etc/system-release-cpe | cut -d. -f1)/apache-arrow-release-latest.rpm
sudo yum install -y --enablerepo=epel arrow-devel # For C++
sudo yum install -y --enablerepo=epel arrow-glib-devel # For GLib (C)
sudo yum install -y --enablerepo=epel arrow-dataset-devel # For Apache Arrow Dataset C++
sudo yum install -y --enablerepo=epel arrow-dataset-glib-devel # For Apache Arrow Dataset GLib (C)
sudo yum install -y --enablerepo=epel parquet-devel # For Apache Parquet C++
sudo yum install -y --enablerepo=epel parquet-glib-devel 
``