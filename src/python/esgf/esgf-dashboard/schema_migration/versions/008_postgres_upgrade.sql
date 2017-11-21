--
-- PostgreSQL database dump
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

drop trigger if exists store_new_entry on esgf_node_manager.access_logging;
drop trigger if exists store_update_entry on esgf_node_manager.access_logging;
drop trigger if exists store_delete_entry on esgf_node_manager.access_logging;

drop function if exists delete_dashboard_queue();
drop function if exists update_dashboard_queue();
drop function if exists store_dashboard_queue();
drop function if exists update_url(integer);

--
-- Fuction to verify if the language exists
--
CREATE OR REPLACE FUNCTION create_language_plpgsql()
RETURNS BOOLEAN AS $$
    CREATE LANGUAGE plpgsql;
    SELECT TRUE;
$$ LANGUAGE SQL;

SELECT CASE WHEN NOT
    (
        SELECT  TRUE AS exists
        FROM    pg_language
        WHERE   lanname = 'plpgsql'
        UNION
        SELECT  FALSE AS exists
        ORDER BY exists DESC
        LIMIT 1
    )
THEN
    create_language_plpgsql()
ELSE
    FALSE
END AS plpgsql_created;

DROP FUNCTION create_language_plpgsql();

--
-- Fuction to verify if a table exists
--
DROP FUNCTION if EXISTS table_exists(TEXT, TEXT) CASCADE;

CREATE FUNCTION table_exists(tablename TEXT, tableschema TEXT) RETURNS integer AS'
DECLARE
tab1 alias for $1;
tab2 alias for $2;
BEGIN
IF EXISTS (SELECT 1 FROM information_schema.tables WHERE table_schema=''esgf_dashboard'' AND table_name=tab1) THEN
  if tab1=''dashboard_queue'' then
    IF not EXISTS (SELECT column_name 
               FROM information_schema.columns 
               WHERE table_schema=''esgf_dashboard'' and table_name=''dashboard_queue'' and column_name=''id_dash'') THEN
       alter table esgf_dashboard.dashboard_queue add column id_dash bigserial PRIMARY KEY ;
    END IF;
    IF not EXISTS (SELECT column_name 
               FROM information_schema.columns 
               WHERE table_schema=''esgf_dashboard'' and table_name=''dashboard_queue'' and column_name=''id'') THEN
       alter table esgf_dashboard.dashboard_queue add column id integer NOT NULL;
    END IF;
    IF not EXISTS (SELECT column_name 
               FROM information_schema.columns 
               WHERE table_schema=''esgf_dashboard'' and table_name=''dashboard_queue'' and column_name=''url_path'') THEN
       alter table esgf_dashboard.dashboard_queue add column url_path character varying NOT NULL;
    END IF;
    IF not EXISTS (SELECT column_name 
               FROM information_schema.columns 
               WHERE table_schema=''esgf_dashboard'' and table_name=''dashboard_queue'' and column_name=''remote_addr'') THEN
       alter table esgf_dashboard.dashboard_queue add column remote_addr character varying NOT NULL;
    END IF;
    IF not EXISTS (SELECT column_name 
               FROM information_schema.columns 
               WHERE table_schema=''esgf_dashboard'' and table_name=''dashboard_queue'' and column_name=''user_id_hash'') THEN
       alter table esgf_dashboard.dashboard_queue add column user_id_hash character varying NOT NULL;
    END IF;
    IF not EXISTS (SELECT column_name 
               FROM information_schema.columns 
               WHERE table_schema=''esgf_dashboard'' and table_name=''dashboard_queue'' and column_name=''user_idp'') THEN
       alter table esgf_dashboard.dashboard_queue add column user_idp character varying NOT NULL;
    END IF;
    IF not EXISTS (SELECT column_name 
               FROM information_schema.columns 
               WHERE table_schema=''esgf_dashboard'' and table_name=''dashboard_queue'' and column_name=''service_type'') THEN
       alter table esgf_dashboard.dashboard_queue add column service_type character varying NOT NULL;
    END IF;
    IF not EXISTS (SELECT column_name 
               FROM information_schema.columns 
               WHERE table_schema=''esgf_dashboard'' and table_name=''dashboard_queue'' and column_name=''success'') THEN
       alter table esgf_dashboard.dashboard_queue add column success boolean;
    END IF;
    IF not EXISTS (SELECT column_name 
               FROM information_schema.columns 
               WHERE table_schema=''esgf_dashboard'' and table_name=''dashboard_queue'' and column_name=''duration'') THEN
       alter table esgf_dashboard.dashboard_queue add column duration double precision;
    END IF;
    IF not EXISTS (SELECT column_name 
               FROM information_schema.columns 
               WHERE table_schema=''esgf_dashboard'' and table_name=''dashboard_queue'' and column_name=''size'') THEN
       alter table esgf_dashboard.dashboard_queue add column size bigint DEFAULT(-1);
    END IF;
    IF not EXISTS (SELECT column_name 
               FROM information_schema.columns 
               WHERE table_schema=''esgf_dashboard'' and table_name=''dashboard_queue'' and column_name=''timestamp'') THEN
       alter table esgf_dashboard.dashboard_queue add column timestamp double precision NOT NULL;
    END IF;
    IF not EXISTS (SELECT column_name 
               FROM information_schema.columns 
               WHERE table_schema=''esgf_dashboard'' and table_name=''dashboard_queue'' and column_name=''processed'') THEN
       alter table esgf_dashboard.dashboard_queue add column processed smallint DEFAULT 0 NOT NULL;
    END IF;
  end if;
  if (tab1=''cross_dmart_project_host_time'' OR tab1=''cross_dmart_project_host_geolocation'' OR tab1=''obs4mips_dmart_clients_host_geolocation'' OR tab1=''obs4mips_dmart_variable_host_time'' OR tab1=''obs4mips_dmart_source_host_time'' OR tab1=''obs4mips_dmart_realm_host_time'' OR tab1=''cmip5_dmart_clients_host_geolocation'' OR tab1=''cmip5_dmart_model_host_time'' OR tab1=''cmip5_dmart_variable_host_time'' OR tab1=''cmip5_dmart_dataset_host_time'' OR tab1=''cmip5_dmart_clients_host_geolocation'' OR tab1=''cmip5_dmart_dataset_host_time'') then
    EXECUTE ''DROP TABLE '' || tab1;
    EXECUTE tab2;
  end if;
  return 0;
ELSE
  EXECUTE tab2;
return 1;
END IF;
END;
'LANGUAGE 'plpgsql';

--
-- Name: dashboard_queue; Type: TABLE; Schema: esgf_dashboard; Owner: dbsuper; Tablespace:
--
select table_exists('dashboard_queue','
CREATE TABLE dashboard_queue (
    id_dash bigserial PRIMARY KEY,                      -- unique id
    id integer NOT NULL,                                -- access_logging table id                       
    url_path character varying NOT NULL,                -- path of the downloaded file
    remote_addr character varying NOT NULL,             -- user ip address
    user_id_hash character varying,                     -- hash code of the user id
    user_idp character varying,                         -- user identity provider
    service_type character varying,                     -- download service type
    success boolean,                                    -- outcome of the download operation
    duration double precision,                          -- duration of the download operation
    size bigint DEFAULT (-1),                           -- file dimensions
    "timestamp" double precision NOT NULL,              -- download time instant
    processed smallint DEFAULT 0 NOT NULL               -- dashboard flag
);

ALTER TABLE esgf_dashboard.dashboard_queue OWNER TO dbsuper;');

--
-- Insert into the dashboard_queue_table a new row stored in the access_logging table
--
DROP FUNCTION if EXISTS store_dashboard_queue() CASCADE;

CREATE FUNCTION store_dashboard_queue() RETURNS trigger AS
$store_new_entry$
declare
BEGIN
-- Update dashboard_queue table
insert into esgf_dashboard.dashboard_queue(id, url_path, remote_addr,
user_id_hash, user_idp, service_type, success, duration, size,
timestamp)values(NEW.id, NEW.url, NEW.remote_addr, NEW.user_id_hash,
NEW.user_idp, NEW.service_type, NEW.success, NEW.duration,
NEW.data_size, NEW.date_fetched);
RETURN NEW;
END
$store_new_entry$ LANGUAGE plpgsql;

DROP FUNCTION if EXISTS update_dashboard_queue() CASCADE;

CREATE FUNCTION update_dashboard_queue() RETURNS trigger AS
$store_update_entry$
declare
url_http varchar;
BEGIN
-- Update dashboard_queue table
update esgf_dashboard.dashboard_queue set success=NEW.success, size=NEW.data_size, duration=NEW.duration WHERE id = OLD.id; 
url_http:=url_path from esgf_dashboard.dashboard_queue WHERE id = OLD.id;
if strpos(url_http,'http')<>0 then
update esgf_dashboard.dashboard_queue set url_path=subquery.url_res
FROM (select file.url as url_res from public.file_version as file,
esgf_dashboard.dashboard_queue as log where log.url_path like '%%'||file.url
and log.url_path=url_http) as subquery where url_path=url_http and id=OLD.id;
end if;
RETURN NEW;
END
$store_update_entry$ LANGUAGE plpgsql;

DROP FUNCTION if EXISTS delete_dashboard_queue() CASCADE;

CREATE FUNCTION delete_dashboard_queue() RETURNS trigger AS
$store_delete_entry$
declare
BEGIN
-- Update dashboard_queue table
delete from esgf_dashboard.dashboard_queue where id=OLD.id;
RETURN NEW;
END
$store_delete_entry$ LANGUAGE plpgsql;

CREATE TRIGGER store_new_entry
AFTER INSERT ON
esgf_node_manager.access_logging
FOR EACH ROW EXECUTE PROCEDURE store_dashboard_queue();
CREATE TRIGGER store_update_entry
AFTER UPDATE ON
esgf_node_manager.access_logging
FOR EACH ROW EXECUTE PROCEDURE update_dashboard_queue();
CREATE TRIGGER store_delete_entry
AFTER DELETE ON
esgf_node_manager.access_logging
FOR EACH ROW EXECUTE PROCEDURE delete_dashboard_queue();

SET search_path = public, pg_catalog;
