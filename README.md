# Titans

微服务架构的整体Quant开发解决方案


## python 虚拟环境

创建虚拟环境
```shell
python3 -m venv dev-env
```

激活虚拟环境
```shell
source dev-env/bin/activate
```

退出虚拟环境
```shell
deactivate
```

使用国内源
```shell
pip3 config set global.index-url https://pypi.tuna.tsinghua.edu.cn/simple dev-env
```

## 编译环境

### dabian 12 & ubuntu

#### 安装 gcc 12 & cmake3
```shell
apt install build-essential
apt install cmake
```

#### 安装 libuv
```shell
apt install libuv1-dev
```

#### 安装hiredis
```shell
apt install libhiredis-dev
```

### 安装 curl
```shell
apt install libcurl4-openssl-dev
```

#### 安装mysql
```shell
apt install libmariadb-dev
```

```shell
#安装
apt-get install libgoogle-glog-dev
#卸载
apt-get remove libgoogle-glog-dev
```

#### 安装 Apache Arrow
```shell
apt update
apt install -y -V ca-certificates lsb-release wget
wget https://apache.jfrog.io/artifactory/arrow/$(lsb_release --id --short | tr 'A-Z' 'a-z')/apache-arrow-apt-source-latest-$(lsb_release --codename --short).deb
apt install -y -V ./apache-arrow-apt-source-latest-$(lsb_release --codename --short).deb
apt update
apt install -y -V libarrow-dev # For C++
apt install -y -V libarrow-glib-dev # For GLib (C)
apt install -y -V libarrow-dataset-dev # For Apache Arrow Dataset C++
apt install -y -V libarrow-dataset-glib-dev # For Apache Arrow Dataset GLib (C)
apt install -y -V libarrow-acero-dev # For Apache Arrow Acero
apt install -y -V libarrow-flight-dev # For Apache Arrow Flight C++
apt install -y -V libarrow-flight-glib-dev # For Apache Arrow Flight GLib (C)
apt install -y -V libarrow-flight-sql-dev # For Apache Arrow Flight SQL C++
apt install -y -V libarrow-flight-sql-glib-dev # For Apache Arrow Flight SQL GLib (C)
apt install -y -V libgandiva-dev # For Gandiva C++
apt install -y -V libgandiva-glib-dev # For Gandiva GLib (C)
apt install -y -V libparquet-dev # For Apache Parquet C++
apt install -y -V libparquet-glib-dev # For Apache Parquet GLib (C)
```

### centos7

### 安装 gcc 9
```shell
yum install centos-release-scl-rh -y
yum install devtoolset-9-build -y
yum install devtoolset-9-gdb* -y
yum install devtoolset-9-gcc devtoolset-9-gcc-c++ -y
source /opt/rh/devtoolset-9/enable
```

### 安装 redis 7 
```shell
yum install -y http://rpms.famillecollet.com/enterprise/remi-release-7.rpm
yum --enablerepo=remi install redis
```

### 安装 libuv
```shell
yum install libuv-devel -y
```

### 安装 glog
```shell
yum install glog-devel -y
yum install gflags-devel -y
```

### 安装 hiredis
```shell
yum install hiredis-devel -y
```

### 安装 curl
```shell
yum install libcurl-devel -y
```

#### 安装mysql
```shell
yum install mariadb-devel -y 
```

### 安装 Apache Arrow
```shell
sudo yum install -y epel-release || sudo yum install -y https://dl.fedoraproject.org/pub/epel/epel-release-latest-$(cut -d: -f5 /etc/system-release-cpe | cut -d. -f1).noarch.rpm
sudo yum install -y https://apache.jfrog.io/artifactory/arrow/centos/$(cut -d: -f5 /etc/system-release-cpe | cut -d. -f1)/apache-arrow-release-latest.rpm
yum install -y --enablerepo=epel arrow-devel # For C++
yum install -y --enablerepo=epel arrow-glib-devel # For GLib (C)
yum install -y --enablerepo=epel arrow-dataset-devel # For Apache Arrow Dataset C++
yum install -y --enablerepo=epel arrow-dataset-glib-devel # For Apache Arrow Dataset GLib (C)
yum install -y --enablerepo=epel parquet-devel # For Apache Parquet C++
yum install -y --enablerepo=epel parquet-glib-devel 
```