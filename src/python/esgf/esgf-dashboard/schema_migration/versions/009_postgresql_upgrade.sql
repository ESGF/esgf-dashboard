DROP TABLE IF EXISTS esgf_dashboard.aggregation_process CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.aggregation_process_planb CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.cpu_metrics CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.federationdw CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.federationdw_planb CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.hasfeed CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.host CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.join1 CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.memory_metrics CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.news CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.project_dash CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.reconciliation_process CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.rssfeed CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.service_instance CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.service_status CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.user1 CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.uses CASCADE;


CREATE TABLE esgf_dashboard.cross_dim_date (
    date_key serial PRIMARY KEY,
    download_date date,
    month smallint,
    year smallint
);
ALTER TABLE esgf_dashboard.cross_dim_date OWNER TO dbsuper;
 
 
CREATE TABLE esgf_dashboard.cross_dim_geolocation (
    geolocation_key bigserial PRIMARY KEY,
    latitude numeric(14,11),
    longitude numeric(14,11),
    country_id integer NOT NULL REFERENCES esgf_dashboard.country
);
ALTER TABLE esgf_dashboard.cross_dim_geolocation OWNER TO dbsuper;
 
 
CREATE TABLE esgf_dashboard.cross_dim_project (
  project_key serial PRIMARY KEY,
  project_name character varying(64)
);
ALTER TABLE esgf_dashboard.cross_dim_project OWNER TO dbsuper;
 
 
CREATE TABLE esgf_dashboard.cross_bridge_project (
  project_key integer NOT NULL REFERENCES esgf_dashboard.cross_dim_project,
  project_group_key integer NOT NULL
);
ALTER TABLE esgf_dashboard.cross_bridge_project OWNER TO dbsuper;
 
/* FACT TABLE */
CREATE TABLE esgf_dashboard.cross_fact_download (
  download_key bigserial PRIMARY KEY,
  size bigint,
  success boolean,
  duration integer,
  replica boolean,
  user_id_hash character varying(64),
  host_name character varying(64),
  user_idp character varying(64),
  hour smallint,
  minute smallint,
  project_group_key integer,
  geolocation_key bigint REFERENCES esgf_dashboard.cross_dim_geolocation,
  date_key integer REFERENCES esgf_dashboard.cross_dim_date,
  id_query integer
);
ALTER TABLE esgf_dashboard.cross_fact_download OWNER TO dbsuper;
 
/* DATA MARTS */
CREATE TABLE esgf_dashboard.cross_dmart_project_host_time (
  dmart_key bigserial PRIMARY KEY,
  total_size bigint,
  number_of_downloads bigint,
  number_of_successful_downloads bigint,
  number_of_replica_downloads bigint,
  average_duration integer,
  number_of_users integer,
  host_name character varying(64),
  project_name character varying(64),
  month smallint,
  year smallint
);
ALTER TABLE esgf_dashboard.cross_dmart_project_host_time OWNER TO dbsuper;
ALTER table esgf_dashboard.cross_dmart_project_host_time add constraint cross_dmart_project_host_time_1 unique (total_size, number_of_downloads, number_of_successful_downloads, number_of_replica_downloads, average_duration, number_of_users, host_name, project_name, month, year);
 
 
CREATE TABLE esgf_dashboard.cross_dmart_project_host_geolocation (
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
   longitude numeric(14,11)
);
ALTER TABLE esgf_dashboard.cross_dmart_project_host_geolocation OWNER TO dbsuper;
ALTER table esgf_dashboard.cross_dmart_project_host_geolocation add constraint cross_dmart_project_host_geolocation_1 unique (total_size, number_of_downloads, number_of_successful_downloads, number_of_replica_downloads, average_duration, number_of_users, host_name, project_name, longitude, latitude);

/* DIMENSION TABLES */
 
CREATE TABLE esgf_dashboard.obs4mips_dim_geolocation (
    geolocation_key bigserial PRIMARY KEY,
    latitude numeric(14,11),
    longitude numeric(14,11),
    country_id integer NOT NULL REFERENCES esgf_dashboard.country
);
ALTER TABLE esgf_dashboard.obs4mips_dim_geolocation OWNER TO dbsuper;
 
CREATE TABLE esgf_dashboard.obs4mips_dim_date (
    date_key serial PRIMARY KEY,
    download_date date,
    month smallint,
    year smallint
);
ALTER TABLE esgf_dashboard.obs4mips_dim_date OWNER TO dbsuper;
 
CREATE TABLE esgf_dashboard.obs4mips_dim_dataset (
    dataset_key bigserial PRIMARY KEY,
    dataset_name character varying(64),
    dataset_version smallint,
    datetime_start character varying(64),
    datetime_stop character varying(64)
);
ALTER TABLE esgf_dashboard.obs4mips_dim_dataset OWNER TO dbsuper;
 
CREATE TABLE esgf_dashboard.obs4mips_dim_file (
    file_key bigserial PRIMARY KEY,
    file_name character varying(64),
    file_size bigint
);
ALTER TABLE esgf_dashboard.obs4mips_dim_file OWNER TO dbsuper;
 
CREATE TABLE esgf_dashboard.obs4mips_dim_institute (
    institute_key serial PRIMARY KEY,
    institute_name character varying(64)
);
ALTER TABLE esgf_dashboard.obs4mips_dim_institute OWNER TO dbsuper;
 
CREATE TABLE esgf_dashboard.obs4mips_bridge_institute (
  institute_key integer NOT NULL REFERENCES esgf_dashboard.obs4mips_dim_institute,
  institute_group_key smallint NOT NULL
);
ALTER TABLE esgf_dashboard.obs4mips_bridge_institute OWNER TO dbsuper;
 
CREATE TABLE esgf_dashboard.obs4mips_dim_variable (
    variable_key serial PRIMARY KEY,
    variable_code character varying(64),
    variable_long_name character varying(64),
    cf_standard_name character varying(64)
);
ALTER TABLE esgf_dashboard.obs4mips_dim_variable OWNER TO dbsuper;
 
CREATE TABLE esgf_dashboard.obs4mips_bridge_variable (
  variable_key integer NOT NULL REFERENCES esgf_dashboard.obs4mips_dim_variable,
  variable_group_key integer NOT NULL
);
ALTER TABLE esgf_dashboard.obs4mips_bridge_variable OWNER TO dbsuper;
 
CREATE TABLE esgf_dashboard.obs4mips_dim_time_frequency (
    time_frequency_key serial PRIMARY KEY,
    time_frequency_value character varying(64)
);
ALTER TABLE esgf_dashboard.obs4mips_dim_time_frequency OWNER TO dbsuper;
 
CREATE TABLE esgf_dashboard.obs4mips_bridge_time_frequency (
  time_frequency_key integer NOT NULL REFERENCES esgf_dashboard.obs4mips_dim_time_frequency,
  time_frequency_group_key integer NOT NULL
);
ALTER TABLE esgf_dashboard.obs4mips_bridge_time_frequency OWNER TO dbsuper;
 
CREATE TABLE esgf_dashboard.obs4mips_dim_processing_level (
    processing_level_key serial PRIMARY KEY,
    processing_level_name character varying(64)
);
ALTER TABLE esgf_dashboard.obs4mips_dim_processing_level OWNER TO dbsuper;
 
CREATE TABLE esgf_dashboard.obs4mips_bridge_processing_level (
  processing_level_key integer NOT NULL REFERENCES esgf_dashboard.obs4mips_dim_processing_level,
  processing_level_group_key smallint NOT NULL
);
ALTER TABLE esgf_dashboard.obs4mips_bridge_processing_level OWNER TO dbsuper;
 
CREATE TABLE esgf_dashboard.obs4mips_dim_source_id (
    source_id_key serial PRIMARY KEY,
    source_id_name character varying(64)
);
ALTER TABLE esgf_dashboard.obs4mips_dim_source_id OWNER TO dbsuper;
 
CREATE TABLE esgf_dashboard.obs4mips_bridge_source_id (
  source_id_key integer NOT NULL REFERENCES esgf_dashboard.obs4mips_dim_source_id,
  source_id_group_key smallint NOT NULL
);
ALTER TABLE esgf_dashboard.obs4mips_bridge_source_id OWNER TO dbsuper;
 
CREATE TABLE esgf_dashboard.obs4mips_dim_realm (
    realm_key serial PRIMARY KEY,
    realm_name character varying(64)
);
ALTER TABLE esgf_dashboard.obs4mips_dim_realm OWNER TO dbsuper;
 
CREATE TABLE esgf_dashboard.obs4mips_bridge_realm (
  realm_key integer NOT NULL REFERENCES esgf_dashboard.obs4mips_dim_realm,
  realm_group_key smallint NOT NULL
);
ALTER TABLE esgf_dashboard.obs4mips_bridge_realm OWNER TO dbsuper;
 
/* FACT TABLE */
CREATE TABLE esgf_dashboard.obs4mips_fact_download (
  download_key bigserial PRIMARY KEY,
  size bigint,
  success boolean,
  duration integer,
  user_id_hash character varying,
  user_idp character varying,
  host_name character varying,
  hour smallint,
  minute smallint,
  index_node_name character varying(64),
  dataset_key bigint REFERENCES esgf_dashboard.obs4mips_dim_dataset,
  file_key bigint REFERENCES esgf_dashboard.obs4mips_dim_file,
  geolocation_key bigint REFERENCES esgf_dashboard.obs4mips_dim_geolocation,
  date_key integer REFERENCES esgf_dashboard.obs4mips_dim_date,
  institute_group_key integer,
  variable_group_key integer,
  time_frequency_group_key integer,
  processing_level_group_key integer,
  source_id_group_key integer,
  realm_group_key integer,
  id_query integer
);
ALTER TABLE esgf_dashboard.obs4mips_fact_download OWNER TO dbsuper;
 
/* DATA MARTS */
CREATE TABLE esgf_dashboard.obs4mips_dmart_clients_host_time_geolocation (
  dmart_key bigserial PRIMARY KEY,
  total_size bigint,
  number_of_downloads bigint,
  number_of_successful_downloads bigint,
  average_duration integer,
  number_of_users integer,
  month smallint,
  year smallint,
  latitude numeric(14,11),
  longitude numeric(14,11),
  host_name character varying(64)
);
ALTER TABLE esgf_dashboard.obs4mips_dmart_clients_host_time_geolocation OWNER TO dbsuper;
ALTER table esgf_dashboard.obs4mips_dmart_clients_host_time_geolocation add constraint obs4mips_dmart_clients_host_time_geolocation_1 unique (total_size, number_of_downloads, number_of_successful_downloads, average_duration, number_of_users, month, year, latitude, longitude, host_name);
 
CREATE TABLE esgf_dashboard.obs4mips_dmart_variable_host_time (
  dmart_key bigserial PRIMARY KEY,
  total_size bigint,
  number_of_downloads bigint,
  number_of_successful_downloads bigint,
  average_duration integer,
  number_of_users integer,
  month smallint,
  year smallint,
  host_name character varying(64),
  variable_code character varying(64),
  variable_long_name character varying(64),
  cf_standard_name character varying(64)
);
ALTER TABLE esgf_dashboard.obs4mips_dmart_variable_host_time OWNER TO dbsuper;
ALTER table esgf_dashboard.obs4mips_dmart_variable_host_time add constraint obs4mips_dmart_variable_host_time_1 unique (total_size, number_of_downloads, number_of_successful_downloads, average_duration, number_of_users, month, year, host_name, variable_code, variable_long_name, cf_standard_name);
 
CREATE TABLE esgf_dashboard.obs4mips_dmart_source_host_time (
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
ALTER TABLE esgf_dashboard.obs4mips_dmart_source_host_time OWNER TO dbsuper;
ALTER table esgf_dashboard.obs4mips_dmart_source_host_time add constraint obs4mips_dmart_source_host_time_1 unique (total_size, number_of_downloads, number_of_successful_downloads, average_duration, number_of_users, month, year, host_name, source_id_name);
 
CREATE TABLE esgf_dashboard.obs4mips_dmart_realm_host_time (
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
ALTER TABLE esgf_dashboard.obs4mips_dmart_realm_host_time OWNER TO dbsuper;
ALTER table esgf_dashboard.obs4mips_dmart_realm_host_time add constraint obs4mips_dmart_realm_host_time_1 unique (total_size, number_of_downloads, number_of_successful_downloads, average_duration, number_of_users, month, year, host_name, realm_name); 

CREATE TABLE esgf_dashboard.obs4mips_dmart_dataset_host_time (
  dmart_key bigserial PRIMARY KEY,
  total_size bigint,
  number_of_downloads bigint,
  number_of_successful_downloads bigint,
  average_duration integer,
  number_of_users integer,
  month smallint,
  year smallint,
  host_name character varying(64),
  dataset_name character varying(64),
  dataset_version smallint,
  datetime_start character varying(64),
  datetime_stop character varying(64)
);
ALTER TABLE esgf_dashboard.obs4mips_dmart_dataset_host_time OWNER TO dbsuper;
ALTER table esgf_dashboard.obs4mips_dmart_dataset_host_time add constraint obs4mips_dmart_dataset_host_time_1 unique (total_size, number_of_downloads, number_of_successful_downloads, average_duration, number_of_users, month, year, host_name, dataset_name, dataset_version, datetime_start, datetime_stop); 
/* DIMENSION TABLES */
 
CREATE TABLE esgf_dashboard.cmip5_dim_geolocation (
    geolocation_key bigserial PRIMARY KEY,
    latitude numeric(14,11),
    longitude numeric(14,11),
    country_id integer NOT NULL REFERENCES esgf_dashboard.country
);
ALTER TABLE esgf_dashboard.cmip5_dim_geolocation OWNER TO dbsuper;
 
CREATE TABLE esgf_dashboard.cmip5_dim_date (
    date_key serial PRIMARY KEY,
    download_date date,
    month smallint,
    year smallint
);
ALTER TABLE esgf_dashboard.cmip5_dim_date OWNER TO dbsuper;
 
CREATE TABLE esgf_dashboard.cmip5_dim_dataset (
    dataset_key bigserial PRIMARY KEY,
    dataset_name character varying(64),
    dataset_version smallint,
    datetime_start character varying(64),
    datetime_stop character varying(64)
);
ALTER TABLE esgf_dashboard.cmip5_dim_dataset OWNER TO dbsuper;
 
CREATE TABLE esgf_dashboard.cmip5_dim_time_frequency (
    time_frequency_key serial PRIMARY KEY,
    time_frequency_value character varying(64)
);
ALTER TABLE esgf_dashboard.cmip5_dim_time_frequency OWNER TO dbsuper;
 
CREATE TABLE esgf_dashboard.cmip5_bridge_time_frequency (
  time_frequency_key integer NOT NULL REFERENCES esgf_dashboard.cmip5_dim_time_frequency,
  time_frequency_group_key integer NOT NULL
);
ALTER TABLE esgf_dashboard.cmip5_bridge_time_frequency OWNER TO dbsuper;
 
CREATE TABLE esgf_dashboard.cmip5_dim_variable (
    variable_key serial PRIMARY KEY,
    variable_code character varying(64),
    variable_long_name character varying(64),
    cf_standard_name character varying(64)
);
ALTER TABLE esgf_dashboard.cmip5_dim_variable OWNER TO dbsuper;
 
CREATE TABLE esgf_dashboard.cmip5_bridge_variable (
  variable_key integer NOT NULL REFERENCES esgf_dashboard.cmip5_dim_variable,
  variable_group_key integer NOT NULL
);
ALTER TABLE esgf_dashboard.cmip5_bridge_variable OWNER TO dbsuper;
 
CREATE TABLE esgf_dashboard.cmip5_dim_experiment (
    experiment_key serial PRIMARY KEY,
    experiment_name character varying(64)
);
ALTER TABLE esgf_dashboard.cmip5_dim_experiment OWNER TO dbsuper;
 
CREATE TABLE esgf_dashboard.cmip5_bridge_experiment (
  experiment_key integer NOT NULL REFERENCES esgf_dashboard.cmip5_dim_experiment,
  experiment_group_key smallint NOT NULL
);
ALTER TABLE esgf_dashboard.cmip5_bridge_experiment OWNER TO dbsuper;
 
CREATE TABLE esgf_dashboard.cmip5_dim_model (
    model_key serial PRIMARY KEY,
    model_name character varying(64)
);
ALTER TABLE esgf_dashboard.cmip5_dim_model OWNER TO dbsuper;
 
CREATE TABLE esgf_dashboard.cmip5_bridge_model (
  model_key integer NOT NULL REFERENCES esgf_dashboard.cmip5_dim_model,
  model_group_key smallint NOT NULL
);
ALTER TABLE esgf_dashboard.cmip5_bridge_model OWNER TO dbsuper;
 
CREATE TABLE esgf_dashboard.cmip5_dim_realm (
    realm_key serial PRIMARY KEY,
    realm_name character varying(64)
);
ALTER TABLE esgf_dashboard.cmip5_dim_realm OWNER TO dbsuper;
 
CREATE TABLE esgf_dashboard.cmip5_bridge_realm (
  realm_key integer NOT NULL REFERENCES esgf_dashboard.cmip5_dim_realm,
  realm_group_key smallint NOT NULL
);
ALTER TABLE esgf_dashboard.cmip5_bridge_realm OWNER TO dbsuper;
 
CREATE TABLE esgf_dashboard.cmip5_dim_institute (
    institute_key serial PRIMARY KEY,
    institute_name character varying(64)
);
ALTER TABLE esgf_dashboard.cmip5_dim_institute OWNER TO dbsuper;
 
CREATE TABLE esgf_dashboard.cmip5_bridge_institute (
  institute_key integer NOT NULL REFERENCES esgf_dashboard.cmip5_dim_institute,
  institute_group_key smallint NOT NULL
);
ALTER TABLE esgf_dashboard.cmip5_bridge_institute OWNER TO dbsuper;
 
 
/* FACT TABLE */
CREATE TABLE esgf_dashboard.cmip5_fact_download (
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
  date_key integer REFERENCES esgf_dashboard.cmip5_dim_date,
  geolocation_key bigint REFERENCES esgf_dashboard.cmip5_dim_geolocation,
  dataset_key bigint REFERENCES esgf_dashboard.cmip5_dim_dataset,
  time_frequency_group_key integer,
  variable_group_key integer,
  experiment_group_key integer,
  model_group_key integer,
  realm_group_key integer,
  institute_group_key integer,
  id_query integer
);
ALTER TABLE esgf_dashboard.cmip5_fact_download OWNER TO dbsuper;
 
/* DATA MARTS */
CREATE TABLE esgf_dashboard.cmip5_dmart_clients_host_time_geolocation (
  dmart_key bigserial PRIMARY KEY,
  total_size bigint,
  number_of_downloads bigint,
  number_of_successful_downloads bigint,
  average_duration integer,
  number_of_users integer,
  number_of_replica_downloads bigint,
  month smallint,
  year smallint,
  latitude numeric(14,11),
  longitude numeric(14,11),
  host_name character varying(64)
);
ALTER TABLE esgf_dashboard.cmip5_dmart_clients_host_time_geolocation OWNER TO dbsuper;
ALTER table esgf_dashboard.cmip5_dmart_clients_host_time_geolocation add constraint cmip5_dmart_clients_host_time_geolocation_1 unique (total_size, number_of_downloads, number_of_successful_downloads, average_duration, number_of_users, number_of_replica_downloads, month, year, latitude, longitude,host_name);
 
CREATE TABLE esgf_dashboard.cmip5_dmart_model_host_time (
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
  model_name character varying(64)
);
ALTER TABLE esgf_dashboard.cmip5_dmart_model_host_time OWNER TO dbsuper;
ALTER table esgf_dashboard.cmip5_dmart_model_host_time add constraint cmip5_dmart_model_host_time_1 unique (total_size, number_of_downloads, number_of_successful_downloads, average_duration, number_of_users, number_of_replica_downloads, month, year, host_name, model_name);

 
CREATE TABLE esgf_dashboard.cmip5_dmart_experiment_host_time (
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
  experiment_name character varying(64)
);
ALTER TABLE esgf_dashboard.cmip5_dmart_experiment_host_time OWNER TO dbsuper;
ALTER table esgf_dashboard.cmip5_dmart_experiment_host_time add constraint cmip5_dmart_experiment_host_time_1 unique (total_size, number_of_downloads, number_of_successful_downloads, average_duration, number_of_users, number_of_replica_downloads, month, year, host_name, experiment_name);
 
CREATE TABLE esgf_dashboard.cmip5_dmart_variable_host_time (
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
  variable_long_name character varying(64),
  cf_standard_name character varying(64)
);
ALTER TABLE esgf_dashboard.cmip5_dmart_variable_host_time OWNER TO dbsuper;
ALTER table esgf_dashboard.cmip5_dmart_variable_host_time add constraint cmip5_dmart_variable_host_time_1 unique (total_size, number_of_downloads, number_of_successful_downloads, average_duration, number_of_users, number_of_replica_downloads, month, year, host_name, variable_code, variable_long_name,cf_standard_name);
 
CREATE TABLE esgf_dashboard.cmip5_dmart_dataset_host_time (
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
  dataset_name character varying(64),
  dataset_version smallint,
  datetime_start character varying(64),
  datetime_stop character varying(64)
);
ALTER TABLE esgf_dashboard.cmip5_dmart_dataset_host_time OWNER TO dbsuper;
ALTER table esgf_dashboard.cmip5_dmart_dataset_host_time add constraint cmip5_dmart_dataset_host_time_1 unique (total_size, number_of_downloads, number_of_successful_downloads, average_duration, number_of_users, number_of_replica_downloads, month, year, host_name, dataset_name, dataset_version, datetime_start, datetime_stop);

CREATE TABLE esgf_dashboard.registry (
  datmart character varying(128) PRIMARY KEY,
  dmart_key integer default 0,
  timestamp integer
);
insert into esgf_dashboard.registry values('esgf_dashboard.cross_dmart_project_host_time',0,0);
insert into esgf_dashboard.registry values('esgf_dashboard.cross_dmart_project_host_geolocation',0,0);
insert into esgf_dashboard.registry values('esgf_dashboard.obs4mips_dmart_clients_host_time_geolocation',0,0);
insert into esgf_dashboard.registry values('esgf_dashboard.obs4mips_dmart_variable_host_time',0,0);
insert into esgf_dashboard.registry values('esgf_dashboard.obs4mips_dmart_source_host_time',0,0);
insert into esgf_dashboard.registry values('esgf_dashboard.obs4mips_dmart_realm_host_time',0,0);
insert into esgf_dashboard.registry values('esgf_dashboard.obs4mips_dmart_dataset_host_time',0,0);
insert into esgf_dashboard.registry values('esgf_dashboard.cmip5_dmart_clients_host_time_geolocation',0,0);
insert into esgf_dashboard.registry values('esgf_dashboard.cmip5_dmart_experiment_host_time',0,0);
insert into esgf_dashboard.registry values('esgf_dashboard.cmip5_dmart_model_host_time',0,0);
insert into esgf_dashboard.registry values('esgf_dashboard.cmip5_dmart_variable_host_time',0,0);
insert into esgf_dashboard.registry values('esgf_dashboard.cmip5_dmart_dataset_host_time',0,0);
SET search_path = public, pg_catalog;