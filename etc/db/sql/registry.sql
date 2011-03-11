-- MySQL dump 10.11
--
-- Host: localhost    Database: registry
-- ------------------------------------------------------
-- Server version	5.0.77

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `country`
--

DROP TABLE IF EXISTS `country`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `country` (
  `id` smallint(5) unsigned NOT NULL auto_increment,
  `name` varchar(64) NOT NULL,
  PRIMARY KEY  (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=489 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `country`
--

LOCK TABLES `country` WRITE;
/*!40000 ALTER TABLE `country` DISABLE KEYS */;
INSERT INTO `country` VALUES (245,'Afghanistan, Islamic State of'),(246,'Albania'),(247,'Algeria'),(248,'American Samoa'),(249,'Andorra, Principality of'),(250,'Angola'),(251,'Anguilla'),(252,'Antarctica'),(253,'Antigua and Barbuda'),(254,'Argentina'),(255,'Armenia'),(256,'Aruba'),(257,'Australia'),(258,'Austria'),(259,'Azerbaidjan'),(260,'Bahamas'),(261,'Bahrain'),(262,'Bangladesh'),(263,'Barbados'),(264,'Belarus'),(265,'Belgium'),(266,'Belize'),(267,'Benin'),(268,'Bermuda'),(269,'Bhutan'),(270,'Bolivia'),(271,'Bosnia-Herzegovina'),(272,'Botswana'),(273,'Bouvet Island'),(274,'Brazil'),(275,'British Indian Ocean Territory'),(276,'Brunei Darussalam'),(277,'Bulgaria'),(278,'Burkina Faso'),(279,'Burundi'),(280,'Cambodia, Kingdom of'),(281,'Cameroon'),(282,'Canada'),(283,'Cape Verde'),(284,'Cayman Islands'),(285,'Central African Republic'),(286,'Chad'),(287,'Chile'),(288,'China'),(289,'Christmas Island'),(290,'Cocos (Keeling) Islands'),(291,'Colombia'),(292,'Comoros'),(293,'Congo'),(294,'Congo, The Democratic Republic of the'),(295,'Cook Islands'),(296,'Costa Rica'),(297,'Croatia'),(298,'Cuba'),(299,'Cyprus'),(300,'Czech Republic'),(301,'Denmark'),(302,'Djibouti'),(303,'Dominica'),(304,'Dominican Republic'),(305,'East Timor'),(306,'Ecuador'),(307,'Egypt'),(308,'El Salvador'),(309,'Equatorial Guinea'),(310,'Eritrea'),(311,'Estonia'),(312,'Ethiopia'),(313,'Falkland Islands'),(314,'Faroe Islands'),(315,'Fiji'),(316,'Finland'),(317,'Former Czechoslovakia'),(318,'Former USSR'),(319,'France'),(320,'France (European Territory)'),(321,'French Guyana'),(322,'French Southern Territories'),(323,'Gabon'),(324,'Gambia'),(325,'Georgia'),(326,'Germany'),(327,'Ghana'),(328,'Gibraltar'),(329,'Great Britain'),(330,'Greece'),(331,'Greenland'),(332,'Grenada'),(333,'Guadeloupe (French)'),(334,'Guam (USA)'),(335,'Guatemala'),(336,'Guinea'),(337,'Guinea Bissau'),(338,'Guyana'),(339,'Haiti'),(340,'Heard and McDonald Islands'),(341,'Holy See (Vatican City State)'),(342,'Honduras'),(343,'Hong Kong'),(344,'Hungary'),(345,'Iceland'),(346,'India'),(347,'Indonesia'),(348,'Iran'),(349,'Iraq'),(350,'Ireland'),(351,'Israel'),(352,'Italy'),(353,'Ivory Coast'),(354,'Jamaica'),(355,'Japan'),(356,'Jordan'),(357,'Kazakhstan'),(358,'Kenya'),(359,'Kiribati'),(360,'Kuwait'),(361,'Kyrgyz Republic (Kyrgyzstan)'),(362,'Laos'),(363,'Latvia'),(364,'Lebanon'),(365,'Lesotho'),(366,'Liberia'),(367,'Libya'),(368,'Liechtenstein'),(369,'Lithuania'),(370,'Luxembourg'),(371,'Macau'),(372,'Macedonia'),(373,'Madagascar'),(374,'Malawi'),(375,'Malaysia'),(376,'Maldives'),(377,'Mali'),(378,'Malta'),(379,'Marshall Islands'),(380,'Martinique (French)'),(381,'Mauritania'),(382,'Mauritius'),(383,'Mayotte'),(384,'Mexico'),(385,'Micronesia'),(386,'Moldavia'),(387,'Monaco'),(388,'Mongolia'),(389,'Montserrat'),(390,'Morocco'),(391,'Mozambique'),(392,'Myanmar'),(393,'Namibia'),(394,'Nauru'),(395,'Nepal'),(396,'Netherlands'),(397,'Netherlands Antilles'),(398,'Neutral Zone'),(399,'New Caledonia (French)'),(400,'New Zealand'),(401,'Nicaragua'),(402,'Niger'),(403,'Nigeria'),(404,'Niue'),(405,'Norfolk Island'),(406,'North Korea'),(407,'Northern Mariana Islands'),(408,'Norway'),(409,'Oman'),(410,'Pakistan'),(411,'Palau'),(412,'Panama'),(413,'Papua New Guinea'),(414,'Paraguay'),(415,'Peru'),(416,'Philippines'),(417,'Pitcairn Island'),(418,'Poland'),(419,'Polynesia (French)'),(420,'Portugal'),(421,'Puerto Rico'),(422,'Qatar'),(423,'Reunion (French)'),(424,'Romania'),(425,'Russian Federation'),(426,'Rwanda'),(427,'S. Georgia & S. Sandwich Isls.'),(428,'Saint Helena'),(429,'Saint Kitts & Nevis Anguilla'),(430,'Saint Lucia'),(431,'Saint Pierre and Miquelon'),(432,'Saint Tome (Sao Tome) and Principe'),(433,'Saint Vincent & Grenadines'),(434,'Samoa'),(435,'San Marino'),(436,'Saudi Arabia'),(437,'Senegal'),(438,'Seychelles'),(439,'Sierra Leone'),(440,'Singapore'),(441,'Slovak Republic'),(442,'Slovenia'),(443,'Solomon Islands'),(444,'Somalia'),(445,'South Africa'),(446,'South Korea'),(447,'Spain'),(448,'Sri Lanka'),(449,'Sudan'),(450,'Suriname'),(451,'Svalbard and Jan Mayen Islands'),(452,'Swaziland'),(453,'Sweden'),(454,'Switzerland'),(455,'Syria'),(456,'Tadjikistan'),(457,'Taiwan'),(458,'Tanzania'),(459,'Thailand'),(460,'Togo'),(461,'Tokelau'),(462,'Tonga'),(463,'Trinidad and Tobago'),(464,'Tunisia'),(465,'Turkey'),(466,'Turkmenistan'),(467,'Turks and Caicos Islands'),(468,'Tuvalu'),(469,'Uganda'),(470,'Ukraine'),(471,'United Arab Emirates'),(472,'United Kingdom'),(473,'United States'),(474,'Uruguay'),(475,'USA Minor Outlying Islands'),(476,'Uzbekistan'),(477,'Vanuatu'),(478,'Venezuela'),(479,'Vietnam'),(480,'Virgin Islands (British)'),(481,'Virgin Islands (USA)'),(482,'Wallis and Futuna Islands'),(483,'Western Sahara'),(484,'Yemen'),(485,'Yugoslavia'),(486,'Zaire'),(487,'Zambia'),(488,'Zimbabwe');
/*!40000 ALTER TABLE `country` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `host`
--

DROP TABLE IF EXISTS `host`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `host` (
  `id` int(10) NOT NULL auto_increment,
  `ip` varchar(64) NOT NULL,
  `name` varchar(255) default NULL,
  `city` varchar(255) default NULL,
  `latitude` decimal(9,6) default NULL,
  `longitude` decimal(9,6) default NULL,
  PRIMARY KEY  (`id`),
  UNIQUE KEY `ip` (`ip`)
) ENGINE=InnoDB AUTO_INCREMENT=31 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `host`
--

LOCK TABLES `host` WRITE;
/*!40000 ALTER TABLE `host` DISABLE KEYS */;
INSERT INTO `host` VALUES (30,'127.0.0.1','localhost','City','40.353291','18.173915');
/*!40000 ALTER TABLE `host` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `join`
--

DROP TABLE IF EXISTS `join`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `join` (
  `idUser` int(11) NOT NULL,
  `idProject` int(11) NOT NULL,
  PRIMARY KEY  (`idUser`,`idProject`),
  KEY `FK_join_2` (`idProject`),
  CONSTRAINT `FK_join_1` FOREIGN KEY (`idUser`) REFERENCES `user` (`id`) ON DELETE CASCADE,
  CONSTRAINT `FK_join_2` FOREIGN KEY (`idProject`) REFERENCES `project` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `join`
--

LOCK TABLES `join` WRITE;
/*!40000 ALTER TABLE `join` DISABLE KEYS */;
INSERT INTO `join` VALUES (1,9),(4,9);
/*!40000 ALTER TABLE `join` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `news`
--

DROP TABLE IF EXISTS `news`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `news` (
  `idNews` int(10) unsigned NOT NULL auto_increment,
  `news` varchar(255) NOT NULL,
  `dateNews` timestamp NOT NULL default CURRENT_TIMESTAMP,
  PRIMARY KEY  (`idNews`)
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `news`
--

LOCK TABLES `news` WRITE;
/*!40000 ALTER TABLE `news` DISABLE KEYS */;
/*!40000 ALTER TABLE `news` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `opendap_node`
--

DROP TABLE IF EXISTS `opendap_node`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `opendap_node` (
  `id` int(11) NOT NULL,
  `url` varchar(255) NOT NULL,
  PRIMARY KEY  (`id`),
  CONSTRAINT `FK_opendap_node_1` FOREIGN KEY (`id`) REFERENCES `service_instance` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `opendap_node`
--

LOCK TABLES `opendap_node` WRITE;
/*!40000 ALTER TABLE `opendap_node` DISABLE KEYS */;
INSERT INTO `opendap_node` VALUES (43,'http://adm02.cmcc.it/thredds');
/*!40000 ALTER TABLE `opendap_node` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `project`
--

DROP TABLE IF EXISTS `project`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `project` (
  `id` int(11) NOT NULL auto_increment,
  `name` varchar(255) NOT NULL,
  `description` varchar(2000) default NULL,
  `startDate` timestamp NOT NULL default CURRENT_TIMESTAMP,
  `endDate` datetime default NULL,
  `regPublic` tinyint(1) NOT NULL default '0',
  PRIMARY KEY  (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=10 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `project`
--

LOCK TABLES `project` WRITE;
/*!40000 ALTER TABLE `project` DISABLE KEYS */;
INSERT INTO `project` VALUES (9,'DataNode Monitoring','Data Node Monitoring','2011-02-24 11:00:26',NULL,0);
/*!40000 ALTER TABLE `project` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `service_instance`
--

DROP TABLE IF EXISTS `service_instance`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `service_instance` (
  `id` int(11) NOT NULL auto_increment,
  `port` int(11) NOT NULL,
  `name` varchar(255) default NULL,
  `institution` varchar(255) default NULL,
  `mail_admin` varchar(255) default NULL,
  `idHost` int(11) NOT NULL,
  PRIMARY KEY  (`id`),
  UNIQUE KEY `host_port` (`idHost`,`port`),
  KEY `fk_host` (`idHost`),
  CONSTRAINT `fk_host` FOREIGN KEY (`idHost`) REFERENCES `host` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=46 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `service_instance`
--

LOCK TABLES `service_instance` WRITE;
/*!40000 ALTER TABLE `service_instance` DISABLE KEYS */;
INSERT INTO `service_instance` VALUES (43,80,'OPeNDAP','Institution','admin_email',30),(44,5432,'Postgresql','Institution','admin_email',30),(45,2811,'GridFTP','Institution','admin_email',30);
/*!40000 ALTER TABLE `service_instance` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `service_status`
--

DROP TABLE IF EXISTS `service_status`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `service_status` (
  `id` bigint(20) NOT NULL auto_increment,
  `timestamp` timestamp NOT NULL default CURRENT_TIMESTAMP,
  `status` enum('Active','Inactive','Timeout','Error') NOT NULL COMMENT 'Don''t change the enumeration value order',
  `elapsedTime` int(11) NOT NULL,
  `idServiceInstance` int(11) NOT NULL,
  PRIMARY KEY  (`id`),
  KEY `status_fk` (`idServiceInstance`),
  CONSTRAINT `status_fk` FOREIGN KEY (`idServiceInstance`) REFERENCES `service_instance` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=848908 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `service_status`
--

LOCK TABLES `service_status` WRITE;
/*!40000 ALTER TABLE `service_status` DISABLE KEYS */;
/*!40000 ALTER TABLE `service_status` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `user`
--

DROP TABLE IF EXISTS `user`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `user` (
  `dn` varchar(255) default NULL,
  `id` int(11) NOT NULL auto_increment,
  `name` varchar(45) NOT NULL,
  `surname` varchar(45) NOT NULL,
  `mail` varchar(45) NOT NULL,
  `username` varchar(25) NOT NULL,
  `password` varchar(32) NOT NULL,
  `registrationDate` timestamp NOT NULL default CURRENT_TIMESTAMP,
  `accountCertified` tinyint(1) NOT NULL default '0',
  `idCountry` smallint(5) unsigned default NULL,
  PRIMARY KEY  (`id`),
  UNIQUE KEY `username` (`username`),
  UNIQUE KEY `dn` (`dn`),
  KEY `FK_user_1` (`idCountry`),
  CONSTRAINT `FK_user_1` FOREIGN KEY (`idCountry`) REFERENCES `country` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB AUTO_INCREMENT=5 DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `user`
--

LOCK TABLES `user` WRITE;
/*!40000 ALTER TABLE `user` DISABLE KEYS */;
INSERT INTO `user` VALUES (NULL,1,'guest','guest','guest@guest.com','guest','d89d9a8b05e6e70040ca5b0e00db6c43','2010-09-28 14:56:06',1,NULL),(NULL,4,'admin','admin','admin@admin.com','admin','a41e064a7c2730f2b180f1df04d3dbb6','2010-09-28 17:39:56',1,352);
/*!40000 ALTER TABLE `user` ENABLE KEYS */;
UNLOCK TABLES;

--
-- Table structure for table `uses`
--

DROP TABLE IF EXISTS `uses`;
SET @saved_cs_client     = @@character_set_client;
SET character_set_client = utf8;
CREATE TABLE `uses` (
  `idProject` int(11) NOT NULL,
  `idServiceInstance` int(11) NOT NULL,
  `startDate` timestamp NOT NULL default CURRENT_TIMESTAMP,
  `endDate` datetime default NULL,
  PRIMARY KEY  (`idProject`,`idServiceInstance`,`startDate`),
  KEY `uses_fk2` (`idServiceInstance`),
  KEY `uses_fk1` (`idProject`),
  CONSTRAINT `uses_fk1` FOREIGN KEY (`idProject`) REFERENCES `project` (`id`) ON DELETE CASCADE,
  CONSTRAINT `uses_fk2` FOREIGN KEY (`idServiceInstance`) REFERENCES `service_instance` (`id`) ON DELETE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=latin1;
SET character_set_client = @saved_cs_client;

--
-- Dumping data for table `uses`
--

LOCK TABLES `uses` WRITE;
/*!40000 ALTER TABLE `uses` DISABLE KEYS */;
INSERT INTO `uses` VALUES (9,43,'2011-02-24 11:10:49',NULL),(9,44,'2011-02-24 14:27:58',NULL),(9,45,'2011-02-24 14:28:02',NULL);
/*!40000 ALTER TABLE `uses` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2011-03-08 14:19:21
