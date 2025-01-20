#!/bin/bash

sudo systemctl status rabbitmq-server.service
sudo systemctl status elasticsearch.service
sudo systemctl status redis-server.service
sudo systemctl status etcd
sudo systemctl status mysql.service

# sudo systemctl start rabbitmq-server.service
# sudo systemctl start elasticsearch.service
# sudo systemctl start redis-server.service
# sudo systemctl start etcd
# sudo systemctl start mysql.service

# sudo systemctl stop rabbitmq-server.service
# sudo systemctl stop elasticsearch.service
# sudo systemctl stop redis-server.service
# sudo systemctl stop etcd
# sudo systemctl stop mysql.service