<?php
    $date = new Hprose\DateTime();
    echo new ReflectionClass('Hprose\Date');
    echo new ReflectionClass('Hprose\DateTime');
    echo $date->timestamp();
?>
