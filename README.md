# mqyog
PHP extension for the message queue [MQyog](https://github.com/yoyopi/MQyog/) 
[![Software License](https://img.shields.io/badge/license-MIT-brightgreen.svg?style=flat)](LICENSE.md)
 
## Features

All features of MQyog are supported:


## Installation

#### Linux

``` sh
sudo apt-get install git-core php5-dev
git clone https://github.com/pluginyog/mqyog --recursive
cd mqyog
phpize
./configure
make
sudo make install
add a new line in php.ini extension=mqyog.so 
```


## Usage

Example:

```php
<?php
$hr = mqyog_connect("127.0.0.1", 9091);
var_dump($hr);
$content = mqyog_get($hr, "testQueue", true, "UTF-8");
var_dump($content);
$putRes = mqyog_put($hr, "testQueue","11");var_dump($putRes);
$status = mqyog_status($hr, "testQueue", true, "UTF-8");
var_dump($status);
$posData = mqyog_view($hr, "testQueue", 1, "UTF-8");
var_dump($posData);
$maxqueueRes = mqyog_maxqueue($hr, "testQueue", 9091);
var_dump($maxqueueRes);

```


## License

The MIT License (MIT). Please see [License File](LICENSE.md) for more information.

