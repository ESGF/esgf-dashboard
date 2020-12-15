CREATE TABLE isenes3_kpis_dw(id BIGSERIAL, size_log bigint, status smallint, duration bigint, log_timestamp bigint, hour NUMERIC(2), day NUMERIC(2), month NUMERIC(2), year NUMERIC(4), country_code VARCHAR(5), hostname VARCHAR(100), entry_log VARCHAR(10485760), entry_type NUMERIC(2), entry_timestamp TIMESTAMP DEFAULT NOW()); 

CREATE TABLE cross_project_dw(id BIGSERIAL, size_solr bigint, size_log bigint, status smallint, duration bigint, replica boolean, log_timestamp bigint, hour NUMERIC(2), day NUMERIC(2), month NUMERIC(2), year NUMERIC(4), country_code VARCHAR(5), hostname VARCHAR(100), entry_log VARCHAR(10485760), entry_type NUMERIC(2), project VARCHAR(20), entry_timestamp TIMESTAMP DEFAULT NOW()); 

CREATE TABLE cmip5_project_specific_dw(id BIGSERIAL, cross_project_id BIGSERIAL, size_solr bigint, size_log bigint, status smallint, duration bigint, replica boolean, log_timestamp bigint, hour NUMERIC(2), day NUMERIC(2), month NUMERIC(2), year NUMERIC(4), country_code VARCHAR(5), hostname VARCHAR(100), realm VARCHAR(255), file_version VARCHAR(32), dataset_id VARCHAR(1024), model VARCHAR(255), experiment VARCHAR(255), institute VARCHAR(255), cf_standard_name VARCHAR(255), variable_code VARCHAR(128), variable_long_name VARCHAR(255), time_frequency VARCHAR(32), service_type VARCHAR(255), entry_log VARCHAR(10485760), entry_type NUMERIC(2), entry_timestamp TIMESTAMP DEFAULT NOW());

CREATE TABLE cmip6_project_specific_dw(id BIGSERIAL, cross_project_id BIGSERIAL,size_solr bigint, size_log bigint, status smallint, duration bigint, replica boolean, log_timestamp bigint, hour NUMERIC(2), day NUMERIC(2), month NUMERIC(2), year NUMERIC(4), country_code VARCHAR(5), hostname VARCHAR(100), realm VARCHAR(255), file_version VARCHAR(32), dataset_id VARCHAR(1024), source_id VARCHAR(255), experiment_id VARCHAR(255), institution_id VARCHAR(255), cf_standard_name VARCHAR(255), variable_code VARCHAR(128), variable_long_name VARCHAR(255), frequency VARCHAR(32), service_type VARCHAR(255), entry_log VARCHAR(10485760), entry_type NUMERIC(2), entry_timestamp TIMESTAMP DEFAULT NOW());

CREATE TABLE cordex_project_specific_dw(id BIGSERIAL, cross_project_id BIGSERIAL,size_solr bigint, size_log bigint, status smallint, duration bigint, replica boolean, log_timestamp bigint, hour NUMERIC(2), day NUMERIC(2), month NUMERIC(2), year NUMERIC(4), country_code VARCHAR(5), hostname VARCHAR(100), file_version VARCHAR(32), domain VARCHAR(50), rcm_name VARCHAR(50), rcm_version VARCHAR(10), dataset_id VARCHAR(1024), driving_model VARCHAR(255), experiment VARCHAR(255), institute VARCHAR(255), cf_standard_name VARCHAR(255), variable_code VARCHAR(128), variable_long_name VARCHAR(255), time_frequency VARCHAR(32), service_type VARCHAR(255), entry_log VARCHAR(10485760), entry_type NUMERIC(2), entry_timestamp TIMESTAMP DEFAULT NOW());

CREATE TABLE obs4mips_project_specific_dw(id BIGSERIAL, cross_project_id BIGSERIAL,size_solr bigint, size_log bigint, status smallint, duration bigint, replica boolean, log_timestamp bigint, hour NUMERIC(2), day NUMERIC(2), month NUMERIC(2), year NUMERIC(4), country_code VARCHAR(5), hostname VARCHAR(100), file_version VARCHAR(32), dataset_id VARCHAR(1024), source_id VARCHAR(255),realm VARCHAR(255),institute VARCHAR(255), cf_standard_name VARCHAR(255), variable_code VARCHAR(128), variable_long_name VARCHAR(255), time_frequency VARCHAR(32), service_type VARCHAR(255), entry_log VARCHAR(10485760), entry_type NUMERIC(2), entry_timestamp TIMESTAMP DEFAULT NOW());





















