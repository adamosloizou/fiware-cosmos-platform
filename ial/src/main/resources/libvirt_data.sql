/*
 * Telefónica Digital - Product Development and Innovation
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 * EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
 * All rights reserved.
 */

--
-- Data for table `SERVERS`
--

LOCK TABLES `SERVERS` WRITE;
/*!40000 ALTER TABLE `SERVERS` DISABLE KEYS */;
INSERT INTO `SERVERS` VALUES
    (
        'andromeda02',
        4,
        'vzbr0',
        'Andromeda 02',
        1,
        'andromeda52',
        'centos-6-cosmos-x86_64',
        'openvz+ssh://cosmos@192.168.63.12/system?socket=/var/run/libvirt/libvirt-sock',
        1,
        3,
        32768,
        '192.168.63.62'
    ),
    (
        'andromeda03',
        4,
        'vzbr0',
        'Andromeda 03',
        1,
        'andromeda53',
        'centos-6-cosmos-x86_64',
        'openvz+ssh://cosmos@192.168.63.13/system?socket=/var/run/libvirt/libvirt-sock',
        2,
        3,
        32768,
        '192.168.63.63'
    ),
    (
        'andromeda04',
        4,
        'vzbr0',
        'Andromeda 04',
        1,
        'andromeda54',
        'centos-6-cosmos-x86_64',
        'openvz+ssh://cosmos@192.168.63.14/system?socket=/var/run/libvirt/libvirt-sock',
        3,
        3,
        32768,
        '192.168.63.64'
    ),
    (
        'andromeda05',
        4,
        'vzbr0',
        'Andromeda 05',
        1,
        'andromeda55',
        'centos-6-cosmos-x86_64',
        'openvz+ssh://cosmos@192.168.63.15/system?socket=/var/run/libvirt/libvirt-sock',
        4,
        3,
        32768,
        '192.168.63.65'
    ),
    (
        'andromeda06',
        4,
        'vzbr0',
        'Andromeda 06',
        1,
        'andromeda56',
        'centos-6-cosmos-x86_64',
        'openvz+ssh://cosmos@192.168.63.16/system?socket=/var/run/libvirt/libvirt-sock',
        5,
        3,
        32768,
        '192.168.63.66'
    );

/*!40000 ALTER TABLE `SERVERS` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;