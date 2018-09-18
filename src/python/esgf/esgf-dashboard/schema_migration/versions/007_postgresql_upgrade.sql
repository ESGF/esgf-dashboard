SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET check_function_bodies = false;
SET client_min_messages = warning;
SET escape_string_warning = off;

SET search_path = esgf_dashboard, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

DROP TABLE IF EXISTS esgf_dashboard.country CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.countries CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.continent CASCADE;

-- ----------------------------
-- Table structure for `continent`
-- ----------------------------
CREATE TABLE esgf_dashboard.continent (
  continent_code character(2) PRIMARY KEY,
  continent_name character varying(255) DEFAULT NULL
);

-- ----------------------------
-- Records of continents
-- ----------------------------
INSERT INTO esgf_dashboard.continent VALUES ('AF', 'Africa');
INSERT INTO esgf_dashboard.continent VALUES ('AN', 'Antarctica');
INSERT INTO esgf_dashboard.continent VALUES ('AS', 'Asia');
INSERT INTO esgf_dashboard.continent VALUES ('EU', 'Europe');
INSERT INTO esgf_dashboard.continent VALUES ('NA', 'North America');
INSERT INTO esgf_dashboard.continent VALUES ('OC', 'Oceania');
INSERT INTO esgf_dashboard.continent VALUES ('SA', 'South America');

-- ----------------------------
-- Table structure for `country`
-- ----------------------------
CREATE TABLE esgf_dashboard.country (
  country_id serial PRIMARY KEY,
  country_code character(2) NOT NULL,
  country_name character varying(64) NOT NULL,
  continent_code character(2) NOT NULL REFERENCES esgf_dashboard.continent
);
 
-- ----------------------------
-- Records of countries
-- ----------------------------
INSERT INTO esgf_dashboard.country VALUES ('1', 'AD', 'Andorra', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('2', 'AE', 'United Arab Emirates', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('3', 'AF', 'Afghanistan', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('4', 'AG', 'Antigua and Barbuda', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('5', 'AI', 'Anguilla', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('6', 'AL', 'Albania', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('7', 'AM', 'Armenia', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('8', 'AN', 'Netherlands Antilles', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('9', 'AO', 'Angola', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('10', 'AQ', 'Antarctica', 'AN');
INSERT INTO esgf_dashboard.country VALUES ('11', 'AR', 'Argentina', 'SA');
INSERT INTO esgf_dashboard.country VALUES ('12', 'AS', 'American Samoa', 'OC');
INSERT INTO esgf_dashboard.country VALUES ('13', 'AT', 'Austria', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('14', 'AU', 'Australia', 'OC');
INSERT INTO esgf_dashboard.country VALUES ('15', 'AW', 'Aruba', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('16', 'AX', 'Åland', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('17', 'AZ', 'Azerbaijan', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('18', 'BA', 'Bosnia and Herzegovina', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('19', 'BB', 'Barbados', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('20', 'BD', 'Bangladesh', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('21', 'BE', 'Belgium', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('22', 'BF', 'Burkina Faso', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('23', 'BG', 'Bulgaria', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('24', 'BH', 'Bahrain', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('25', 'BI', 'Burundi', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('26', 'BJ', 'Benin', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('27', 'BL', 'Saint Barthélemy', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('28', 'BM', 'Bermuda', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('29', 'BN', 'Brunei Darussalam', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('30', 'BO', 'Bolivia', 'SA');
INSERT INTO esgf_dashboard.country VALUES ('31', 'BR', 'Brazil', 'SA');
INSERT INTO esgf_dashboard.country VALUES ('32', 'BS', 'Bahamas', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('33', 'BT', 'Bhutan', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('34', 'BV', 'Bouvet Island', 'AN');
INSERT INTO esgf_dashboard.country VALUES ('35', 'BW', 'Botswana', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('36', 'BY', 'Belarus', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('37', 'BZ', 'Belize', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('38', 'CA', 'Canada', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('39', 'CC', 'Cocos (Keeling) Islands', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('40', 'CD', 'Congo (Kinshasa)', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('41', 'CF', 'Central African Republic', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('42', 'CG', 'Congo (Brazzaville)', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('43', 'CH', 'Switzerland', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('44', 'CI', 'Côte d''Ivoire', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('45', 'CK', 'Cook Islands', 'OC');
INSERT INTO esgf_dashboard.country VALUES ('46', 'CL', 'Chile', 'SA');
INSERT INTO esgf_dashboard.country VALUES ('47', 'CM', 'Cameroon', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('48', 'CN', 'China', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('49', 'CO', 'Colombia', 'SA');
INSERT INTO esgf_dashboard.country VALUES ('50', 'CR', 'Costa Rica', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('51', 'CU', 'Cuba', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('52', 'CV', 'Cape Verde', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('53', 'CX', 'Christmas Island', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('54', 'CY', 'Cyprus', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('55', 'CZ', 'Czech Republic', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('56', 'DE', 'Germany', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('57', 'DJ', 'Djibouti', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('58', 'DK', 'Denmark', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('59', 'DM', 'Dominica', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('60', 'DO', 'Dominican Republic', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('61', 'DZ', 'Algeria', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('62', 'EC', 'Ecuador', 'SA');
INSERT INTO esgf_dashboard.country VALUES ('63', 'EE', 'Estonia', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('64', 'EG', 'Egypt', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('65', 'EH', 'Western Sahara', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('66', 'ER', 'Eritrea', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('67', 'ES', 'Spain', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('68', 'ET', 'Ethiopia', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('69', 'FI', 'Finland', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('70', 'FJ', 'Fiji', 'OC');
INSERT INTO esgf_dashboard.country VALUES ('71', 'FK', 'Falkland Islands', 'SA');
INSERT INTO esgf_dashboard.country VALUES ('72', 'FM', 'Micronesia', 'OC');
INSERT INTO esgf_dashboard.country VALUES ('73', 'FO', 'Faroe Islands', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('74', 'FR', 'France', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('75', 'GA', 'Gabon', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('76', 'GB', 'United Kingdom', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('77', 'GD', 'Grenada', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('78', 'GE', 'Georgia', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('79', 'GF', 'French Guiana', 'SA');
INSERT INTO esgf_dashboard.country VALUES ('80', 'GG', 'Guernsey', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('81', 'GH', 'Ghana', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('82', 'GI', 'Gibraltar', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('83', 'GL', 'Greenland', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('84', 'GM', 'Gambia', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('85', 'GN', 'Guinea', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('86', 'GP', 'Guadeloupe', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('87', 'GQ', 'Equatorial Guinea', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('88', 'GR', 'Greece', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('89', 'GS', 'South Georgia and South Sandwich Islands', 'AN');
INSERT INTO esgf_dashboard.country VALUES ('90', 'GT', 'Guatemala', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('91', 'GU', 'Guam', 'OC');
INSERT INTO esgf_dashboard.country VALUES ('92', 'GW', 'Guinea-Bissau', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('93', 'GY', 'Guyana', 'SA');
INSERT INTO esgf_dashboard.country VALUES ('94', 'HK', 'Hong Kong', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('95', 'HM', 'Heard and McDonald Islands', 'AN');
INSERT INTO esgf_dashboard.country VALUES ('96', 'HN', 'Honduras', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('97', 'HR', 'Croatia', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('98', 'HT', 'Haiti', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('99', 'HU', 'Hungary', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('100', 'ID', 'Indonesia', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('101', 'IE', 'Ireland', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('102', 'IL', 'Israel', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('103', 'IM', 'Isle of Man', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('104', 'IN', 'India', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('105', 'IO', 'British Indian Ocean Territory', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('106', 'IQ', 'Iraq', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('107', 'IR', 'Iran', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('108', 'IS', 'Iceland', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('109', 'IT', 'Italy', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('110', 'JE', 'Jersey', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('111', 'JM', 'Jamaica', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('112', 'JO', 'Jordan', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('113', 'JP', 'Japan', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('114', 'KE', 'Kenya', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('115', 'KG', 'Kyrgyzstan', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('116', 'KH', 'Cambodia', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('117', 'KI', 'Kiribati', 'OC');
INSERT INTO esgf_dashboard.country VALUES ('118', 'KM', 'Comoros', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('119', 'KN', 'Saint Kitts and Nevis', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('120', 'KP', 'Korea, North', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('121', 'KR', 'Korea, South', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('122', 'KW', 'Kuwait', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('123', 'KY', 'Cayman Islands', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('124', 'KZ', 'Kazakhstan', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('125', 'LA', 'Laos', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('126', 'LB', 'Lebanon', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('127', 'LC', 'Saint Lucia', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('128', 'LI', 'Liechtenstein', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('129', 'LK', 'Sri Lanka', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('130', 'LR', 'Liberia', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('131', 'LS', 'Lesotho', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('132', 'LT', 'Lithuania', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('133', 'LU', 'Luxembourg', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('134', 'LV', 'Latvia', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('135', 'LY', 'Libya', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('136', 'MA', 'Morocco', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('137', 'MC', 'Monaco', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('138', 'MD', 'Moldova', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('139', 'ME', 'Montenegro', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('140', 'MF', 'Saint Martin (French part)', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('141', 'MG', 'Madagascar', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('142', 'MH', 'Marshall Islands', 'OC');
INSERT INTO esgf_dashboard.country VALUES ('143', 'MK', 'Macedonia', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('144', 'ML', 'Mali', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('145', 'MM', 'Myanmar', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('146', 'MN', 'Mongolia', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('147', 'MO', 'Macau', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('148', 'MP', 'Northern Mariana Islands', 'OC');
INSERT INTO esgf_dashboard.country VALUES ('149', 'MQ', 'Martinique', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('150', 'MR', 'Mauritania', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('151', 'MS', 'Montserrat', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('152', 'MT', 'Malta', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('153', 'MU', 'Mauritius', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('154', 'MV', 'Maldives', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('155', 'MW', 'Malawi', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('156', 'MX', 'Mexico', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('157', 'MY', 'Malaysia', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('158', 'MZ', 'Mozambique', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('159', 'NA', 'Namibia', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('160', 'NC', 'New Caledonia', 'OC');
INSERT INTO esgf_dashboard.country VALUES ('161', 'NE', 'Niger', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('162', 'NF', 'Norfolk Island', 'OC');
INSERT INTO esgf_dashboard.country VALUES ('163', 'NG', 'Nigeria', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('164', 'NI', 'Nicaragua', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('165', 'NL', 'Netherlands', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('166', 'NO', 'Norway', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('167', 'NP', 'Nepal', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('168', 'NR', 'Nauru', 'OC');
INSERT INTO esgf_dashboard.country VALUES ('169', 'NU', 'Niue', 'OC');
INSERT INTO esgf_dashboard.country VALUES ('170', 'NZ', 'New Zealand', 'OC');
INSERT INTO esgf_dashboard.country VALUES ('171', 'OM', 'Oman', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('172', 'PA', 'Panama', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('173', 'PE', 'Peru', 'SA');
INSERT INTO esgf_dashboard.country VALUES ('174', 'PF', 'French Polynesia', 'OC');
INSERT INTO esgf_dashboard.country VALUES ('175', 'PG', 'Papua New Guinea', 'OC');
INSERT INTO esgf_dashboard.country VALUES ('176', 'PH', 'Philippines', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('177', 'PK', 'Pakistan', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('178', 'PL', 'Poland', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('179', 'PM', 'Saint Pierre and Miquelon', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('180', 'PN', 'Pitcairn', 'OC');
INSERT INTO esgf_dashboard.country VALUES ('181', 'PR', 'Puerto Rico', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('182', 'PS', 'Palestine', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('183', 'PT', 'Portugal', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('184', 'PW', 'Palau', 'OC');
INSERT INTO esgf_dashboard.country VALUES ('185', 'PY', 'Paraguay', 'SA');
INSERT INTO esgf_dashboard.country VALUES ('186', 'QA', 'Qatar', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('187', 'RE', 'Reunion', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('188', 'RO', 'Romania', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('189', 'RS', 'Serbia', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('190', 'RU', 'Russian Federation', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('191', 'RW', 'Rwanda', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('192', 'SA', 'Saudi Arabia', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('193', 'SB', 'Solomon Islands', 'OC');
INSERT INTO esgf_dashboard.country VALUES ('194', 'SC', 'Seychelles', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('195', 'SD', 'Sudan', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('196', 'SE', 'Sweden', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('197', 'SG', 'Singapore', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('198', 'SH', 'Saint Helena', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('199', 'SI', 'Slovenia', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('200', 'SJ', 'Svalbard and Jan Mayen Islands', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('201', 'SK', 'Slovakia', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('202', 'SL', 'Sierra Leone', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('203', 'SM', 'San Marino', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('204', 'SN', 'Senegal', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('205', 'SO', 'Somalia', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('206', 'SR', 'Suriname', 'SA');
INSERT INTO esgf_dashboard.country VALUES ('207', 'ST', 'Sao Tome and Principe', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('208', 'SV', 'El Salvador', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('209', 'SY', 'Syria', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('210', 'SZ', 'Swaziland', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('211', 'TC', 'Turks and Caicos Islands', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('212', 'TD', 'Chad', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('213', 'TF', 'French Southern Lands', 'AN');
INSERT INTO esgf_dashboard.country VALUES ('214', 'TG', 'Togo', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('215', 'TH', 'Thailand', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('216', 'TJ', 'Tajikistan', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('217', 'TK', 'Tokelau', 'OC');
INSERT INTO esgf_dashboard.country VALUES ('218', 'TL', 'Timor-Leste', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('219', 'TM', 'Turkmenistan', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('220', 'TN', 'Tunisia', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('221', 'TO', 'Tonga', 'OC');
INSERT INTO esgf_dashboard.country VALUES ('222', 'TR', 'Turkey', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('223', 'TT', 'Trinidad and Tobago', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('224', 'TV', 'Tuvalu', 'OC');
INSERT INTO esgf_dashboard.country VALUES ('225', 'TW', 'Taiwan', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('226', 'TZ', 'Tanzania', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('227', 'UA', 'Ukraine', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('228', 'UG', 'Uganda', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('229', 'UM', 'United States Minor Outlying Islands', 'OC');
INSERT INTO esgf_dashboard.country VALUES ('230', 'US', 'United States of America', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('231', 'UY', 'Uruguay', 'SA');
INSERT INTO esgf_dashboard.country VALUES ('232', 'UZ', 'Uzbekistan', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('233', 'VA', 'Vatican City', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('234', 'VC', 'Saint Vincent and the Grenadines', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('235', 'VE', 'Venezuela', 'SA');
INSERT INTO esgf_dashboard.country VALUES ('236', 'VG', 'Virgin Islands, British', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('237', 'VI', 'Virgin Islands, U.S.', 'NA');
INSERT INTO esgf_dashboard.country VALUES ('238', 'VN', 'Vietnam', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('239', 'VU', 'Vanuatu', 'OC');
INSERT INTO esgf_dashboard.country VALUES ('240', 'WF', 'Wallis and Futuna Islands', 'OC');
INSERT INTO esgf_dashboard.country VALUES ('241', 'WS', 'Samoa', 'OC');
INSERT INTO esgf_dashboard.country VALUES ('242', 'YE', 'Yemen', 'AS');
INSERT INTO esgf_dashboard.country VALUES ('243', 'YT', 'Mayotte', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('244', 'ZA', 'South Africa', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('245', 'ZM', 'Zambia', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('246', 'ZW', 'Zimbabwe', 'AF');
INSERT INTO esgf_dashboard.country VALUES ('247', 'EU', 'No country', 'EU');
INSERT INTO esgf_dashboard.country VALUES ('248', 'AP', 'No country', 'AS');

ALTER TABLE esgf_dashboard.continent ADD COLUMN latitude numeric(14,11);
ALTER TABLE esgf_dashboard.continent ADD COLUMN longitude numeric(14,11);

UPDATE esgf_dashboard.continent set latitude = 7.18805555556 WHERE continent_code='AF';
UPDATE esgf_dashboard.continent set longitude = 21.0936111111 WHERE continent_code='AF';

UPDATE esgf_dashboard.continent set latitude = 29.8405555556 WHERE continent_code='AS';
UPDATE esgf_dashboard.continent set longitude = 89.2966666667 WHERE continent_code='AS';

UPDATE esgf_dashboard.continent set latitude = 48.6908333333 WHERE continent_code='EU';
UPDATE esgf_dashboard.continent set longitude = 9.14055555556 WHERE continent_code='EU';

UPDATE esgf_dashboard.continent set latitude = 46.0730555556 WHERE continent_code='NA';
UPDATE esgf_dashboard.continent set longitude = -100.546666667 WHERE continent_code='NA';

UPDATE esgf_dashboard.continent set latitude = -18.3127777778 WHERE continent_code='OC';
UPDATE esgf_dashboard.continent set longitude = 138.515555556 WHERE continent_code='OC';

UPDATE esgf_dashboard.continent set latitude = -14.6047222222 WHERE continent_code='SA';
UPDATE esgf_dashboard.continent set longitude = -57.6561111111 WHERE continent_code='SA';

UPDATE esgf_dashboard.continent set latitude = -83.3594444444 WHERE continent_code='AN';
UPDATE esgf_dashboard.continent set longitude = 16.5233333333 WHERE continent_code='AN';
SET search_path = public, pg_catalog;