--
-- PostgreSQL updates for OpenID blacklist
--

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = off;
SET check_function_bodies = false;
SET client_min_messages = warning;
SET escape_string_warning = off;

SET search_path = esgf_dashboard, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

/* DIMENSION TABLES */
DROP TABLE IF EXISTS esgf_dashboard.cmip6_dim_geolocation CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_dim_date CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_dim_dataset CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_dim_frequency CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_bridge_frequency CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_dim_variable CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_bridge_variable CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_dim_experiment_id CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_bridge_experiment_id CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_dim_realm CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_bridge_realm CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_dim_institution_id CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_bridge_institution_id CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_dim_source_id CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_bridge_source_id CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_fact_download CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_dmart_clients_host_geolocation CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_dmart_experiment_host_time CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_dmart_variable_host_time CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_dmart_dataset_host_time CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_dmart_source_host_time CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cmip6_dmart_realm_host_time CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cross_dmart_project_host_geolocation CASCADE;

select table_exists('cmip6_dim_geolocation','
CREATE TABLE esgf_dashboard.cmip6_dim_geolocation (
    geolocation_key bigserial PRIMARY KEY,
    latitude numeric(14,11),
    longitude numeric(14,11),
    country_id integer NOT NULL REFERENCES esgf_dashboard.country
);
ALTER TABLE esgf_dashboard.cmip6_dim_geolocation OWNER TO dbsuper;');

select table_exists('cmip6_dim_date', 'CREATE TABLE esgf_dashboard.cmip6_dim_date (
    date_key serial PRIMARY KEY,
    download_date date,
    month smallint,
    year smallint
);
ALTER TABLE esgf_dashboard.cmip6_dim_date OWNER TO dbsuper;');

select table_exists('cmip6_dim_dataset', 'CREATE TABLE esgf_dashboard.cmip6_dim_dataset (
    dataset_key bigserial PRIMARY KEY,
    dataset_name character varying(128),
    dataset_version smallint,
    datetime_start character varying(64),
    datetime_stop character varying(64)
);
ALTER TABLE esgf_dashboard.cmip6_dim_dataset OWNER TO dbsuper;');

select table_exists('cmip6_dim_frequency', 'CREATE TABLE esgf_dashboard.cmip6_dim_frequency (
    frequency_key serial PRIMARY KEY,
    frequency_value character varying(64)
);
ALTER TABLE esgf_dashboard.cmip6_dim_frequency OWNER TO dbsuper;');

select table_exists('cmip6_bridge_frequency', 'CREATE TABLE esgf_dashboard.cmip6_bridge_frequency (
  frequency_key integer NOT NULL REFERENCES esgf_dashboard.cmip6_dim_frequency,
  frequency_group_key integer NOT NULL
);
ALTER TABLE esgf_dashboard.cmip6_bridge_frequency OWNER TO dbsuper;');

select table_exists('cmip6_dim_variable', 'CREATE TABLE esgf_dashboard.cmip6_dim_variable (
    variable_key serial PRIMARY KEY,
    variable_code character varying(64),
    variable_long_name character varying(128),
    cf_standard_name character varying(64)
);
ALTER TABLE esgf_dashboard.cmip6_dim_variable OWNER TO dbsuper;');

select table_exists('cmip6_bridge_variable', 'CREATE TABLE esgf_dashboard.cmip6_bridge_variable (
  variable_key integer NOT NULL REFERENCES esgf_dashboard.cmip6_dim_variable,
  variable_group_key integer NOT NULL
);
ALTER TABLE esgf_dashboard.cmip6_bridge_variable OWNER TO dbsuper;');

select table_exists('cmip6_dim_experiment_id', 'CREATE TABLE esgf_dashboard.cmip6_dim_experiment_id (
    experiment_id_key serial PRIMARY KEY,
    experiment_id_name character varying(64)
);
ALTER TABLE esgf_dashboard.cmip6_dim_experiment_id OWNER TO dbsuper;');

select table_exists('cmip6_bridge_experiment_id', 'CREATE TABLE esgf_dashboard.cmip6_bridge_experiment_id (
  experiment_id_key integer NOT NULL REFERENCES esgf_dashboard.cmip6_dim_experiment_id,
  experiment_id_group_key smallint NOT NULL
);
ALTER TABLE esgf_dashboard.cmip6_bridge_experiment_id OWNER TO dbsuper;');

select table_exists('cmip6_dim_realm', 'CREATE TABLE esgf_dashboard.cmip6_dim_realm (
    realm_key serial PRIMARY KEY,
    realm_name character varying(64)
);
ALTER TABLE esgf_dashboard.cmip6_dim_realm OWNER TO dbsuper;');

select table_exists('cmip6_bridge_realm', 'CREATE TABLE esgf_dashboard.cmip6_bridge_realm (
  realm_key integer NOT NULL REFERENCES esgf_dashboard.cmip6_dim_realm,
  realm_group_key smallint NOT NULL
);
ALTER TABLE esgf_dashboard.cmip6_bridge_realm OWNER TO dbsuper;');

select table_exists('cmip6_dim_institution_id', 'CREATE TABLE esgf_dashboard.cmip6_dim_institution_id (
    institution_id_key serial PRIMARY KEY,
    institution_id_name character varying(64)
);
ALTER TABLE esgf_dashboard.cmip6_dim_institution_id OWNER TO dbsuper;');

select table_exists('cmip6_bridge_institution_id', 'CREATE TABLE esgf_dashboard.cmip6_bridge_institution_id (
  institution_id_key integer NOT NULL REFERENCES esgf_dashboard.cmip6_dim_institution_id,
  institution_id_group_key smallint NOT NULL
);
ALTER TABLE esgf_dashboard.cmip6_bridge_institution_id OWNER TO dbsuper;');

select table_exists('cmip6_dim_source_id', 'CREATE TABLE esgf_dashboard.cmip6_dim_source_id (
    source_id_key serial PRIMARY KEY,
    source_id_name character varying(64)
);
ALTER TABLE esgf_dashboard.cmip6_dim_source_id OWNER TO dbsuper;');
 
select table_exists('cmip6_bridge_source_id', 'CREATE TABLE esgf_dashboard.cmip6_bridge_source_id (
  source_id_key integer NOT NULL REFERENCES esgf_dashboard.cmip6_dim_source_id,
  source_id_group_key smallint NOT NULL
);
ALTER TABLE esgf_dashboard.cmip6_bridge_source_id OWNER TO dbsuper;');


/* FACT TABLE */

select table_exists('cmip6_fact_download', 'CREATE TABLE esgf_dashboard.cmip6_fact_download (
  download_key bigserial PRIMARY KEY,
  size bigint,
  success boolean,
  duration integer,
  replica boolean,
  host_name character varying,
  hour smallint,
  minute smallint,
  user_id_hash character varying,
  user_idp character varying,
  date_key integer REFERENCES esgf_dashboard.cmip6_dim_date,
  geolocation_key bigint REFERENCES esgf_dashboard.cmip6_dim_geolocation,
  dataset_key bigint REFERENCES esgf_dashboard.cmip6_dim_dataset,
  source_id_group_key integer,
  frequency_group_key integer,
  variable_group_key integer,
  experiment_id_group_key integer,
  realm_group_key integer,
  institution_id_group_key integer,
  id_query integer
);
ALTER TABLE esgf_dashboard.cmip6_fact_download OWNER TO dbsuper;');

/* DATA MARTS */

select table_exists('cmip6_dmart_clients_host_geolocation', 'CREATE TABLE esgf_dashboard.cmip6_dmart_clients_host_geolocation (
  dmart_key bigserial PRIMARY KEY,
  total_size bigint,
  number_of_downloads bigint,
  number_of_successful_downloads bigint,
  average_duration integer,
  number_of_users integer,
  number_of_replica_downloads bigint,
  latitude numeric(14,11),
  longitude numeric(14,11),
  country_id integer,
  host_name character varying(64)
);
ALTER TABLE esgf_dashboard.cmip6_dmart_clients_host_geolocation OWNER TO dbsuper;');

select table_exists('cmip6_dmart_experiment_host_time', 'CREATE TABLE esgf_dashboard.cmip6_dmart_experiment_host_time (
  dmart_key bigserial PRIMARY KEY,
  total_size bigint,
  number_of_downloads bigint,
  number_of_successful_downloads bigint,
  average_duration integer,
  number_of_users integer,
  number_of_replica_downloads bigint,
  month smallint,
  year smallint,
  host_name character varying(64),
  experiment_id_name character varying(64)
);
ALTER TABLE esgf_dashboard.cmip6_dmart_experiment_host_time OWNER TO dbsuper;');

select table_exists('cmip6_dmart_variable_host_time', 'CREATE TABLE esgf_dashboard.cmip6_dmart_variable_host_time (
  dmart_key bigserial PRIMARY KEY,
  total_size bigint,
  number_of_downloads bigint,
  number_of_successful_downloads bigint,
  average_duration integer,
  number_of_users integer,
  number_of_replica_downloads bigint,
  month smallint,
  year smallint,
  host_name character varying(64),
  variable_code character varying(64),
  variable_long_name character varying(128),
  cf_standard_name character varying(64)
);
ALTER TABLE esgf_dashboard.cmip6_dmart_variable_host_time OWNER TO dbsuper;');

select table_exists('cmip6_dmart_dataset_host_time', 'CREATE TABLE esgf_dashboard.cmip6_dmart_dataset_host_time (
  dmart_key bigserial PRIMARY KEY,
  total_size bigint,
  number_of_downloads bigint,
  number_of_successful_downloads bigint,
  average_duration integer,
  number_of_users integer,
  number_of_replica_downloads bigint,
  month smallint,
  year smallint,
  host_name character varying(64),
  dataset_name character varying(128),
  dataset_version smallint,
  datetime_start character varying(64),
  datetime_stop character varying(64)
);
ALTER TABLE esgf_dashboard.cmip6_dmart_dataset_host_time OWNER TO dbsuper;');

select table_exists('cmip6_dmart_source_host_time', 'CREATE TABLE esgf_dashboard.cmip6_dmart_source_host_time (
  dmart_key bigserial PRIMARY KEY,
  total_size bigint,
  number_of_downloads bigint,
  number_of_successful_downloads bigint,
  average_duration integer,
  number_of_users integer,
  month smallint,
  year smallint,
  host_name character varying(64),
  source_id_name character varying(64)
);
ALTER TABLE esgf_dashboard.cmip6_dmart_source_host_time OWNER TO dbsuper;');
 
select table_exists('cmip6_dmart_realm_host_time', 'CREATE TABLE esgf_dashboard.cmip6_dmart_realm_host_time (
  dmart_key bigserial PRIMARY KEY,
  total_size bigint,
  number_of_downloads bigint,
  number_of_successful_downloads bigint,
  average_duration integer,
  number_of_users integer,
  month smallint,
  year smallint,
  host_name character varying(64),
  realm_name character varying(64)
);
ALTER TABLE esgf_dashboard.cmip6_dmart_realm_host_time OWNER TO dbsuper;');

select table_exists('cross_dmart_project_host_geolocation', 'CREATE TABLE esgf_dashboard.cross_dmart_project_host_geolocation (
   dmart_key bigserial PRIMARY KEY,
   total_size bigint,
   number_of_downloads bigint,
   number_of_successful_downloads bigint,
   number_of_replica_downloads bigint,
   average_duration integer,
   number_of_users integer,
   host_name character varying(64),
   project_name character varying(64),
   latitude numeric(14,11),
   longitude numeric(14,11),
   country_id integer
);
ALTER TABLE esgf_dashboard.cross_dmart_project_host_geolocation OWNER TO dbsuper;');

insert into esgf_dashboard.registry values('esgf_dashboard.cmip6_dmart_clients_host_geolocation',0,0);
insert into esgf_dashboard.registry values('esgf_dashboard.cmip6_dmart_experiment_host_time',0,0);
insert into esgf_dashboard.registry values('esgf_dashboard.cmip6_dmart_variable_host_time',0,0);
insert into esgf_dashboard.registry values('esgf_dashboard.cmip6_dmart_dataset_host_time',0,0);
insert into esgf_dashboard.registry values('esgf_dashboard.cmip6_dmart_source_host_time',0,0);
insert into esgf_dashboard.registry values('esgf_dashboard.cmip6_dmart_realm_host_time',0,0);

SET search_path = public, pg_catalog;
