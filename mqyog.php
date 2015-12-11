<?php


$hr = mqyog_connect("127.0.0.1", 9091);
var_dump($hr);
$content = mqyog_get($hr, "testQueue", true, "UTF-8");
var_dump($content);
$putRes = mqyog_put($hr, "testQueue","11");var_dump($putRes);
$status = mqyog_status($hr, "testQueue", true, "UTF-8");
var_dump($status);
#$posData = mqyog_view($hr, "testQueue", 1, "UTF-8");
#var_dump($posData);
#$maxqueueRes = mqyog_maxqueue($hr, "testQueue", 9091);
#var_dump($maxqueueRes);
