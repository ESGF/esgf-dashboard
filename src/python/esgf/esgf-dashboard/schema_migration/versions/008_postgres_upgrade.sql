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

--
-- Drop the dashboard_queue table
--

drop table if exists esgf_dashboard.dashboard_queue;
--
-- Name: dashboard_queue; Type: TABLE; Schema: esgf_dashboard; Owner: dbsuper; Tablespace:
--

CREATE TABLE dashboard_queue (
    id integer NOT NULL,                                                    -- id univoco della tupla
    url_path character varying NOT NULL,                                      -- path del file scaricato
    remote_addr character varying NOT NULL,                                   -- ip dell'utente
    user_id_hash character varying,                                           -- hash dell'utente
    user_idp character varying,                                               -- identity provider dell'utente
    service_type character varying,                                           -- tipo di servizio da cui è effettuato il download (i.e. thredds)
    success boolean,                                                          -- esito dell'operazione di download
    duration double precision,                                                -- durata dell'operazione
    size bigint DEFAULT (-1),                                                 -- dimensioni del file
    "timestamp" double precision NOT NULL,                                    -- timestamp
    processed smallint DEFAULT 0 NOT NULL                                     -- preso in carico dalla dashboard
);

--
-- Name: dashboard_queue_id_seq; Type: SEQUENCE; Schema: esgf_dashboard; Owner: dbsuper;
--

CREATE SEQUENCE dashboard_queue_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MAXVALUE
    NO MINVALUE
    CACHE 1;

--
-- Name: dashboard_queue_id_seq; Type: SEQUENCE OWNED BY; Schema: esgf_dashboard; Owner: dbsuper;
--

ALTER SEQUENCE dashboard_queue_id_seq OWNED BY dashboard_queue.id;

--
-- Name: id; Type: DEFAULT; Schema: esgf_dashboard; Owner: dbsuper;
--

ALTER TABLE dashboard_queue ALTER COLUMN id SET DEFAULT nextval('dashboard_queue_id_seq'::regclass);

--
-- Name: dashboard_queue_pkey; Type: CONSTRAINT; Schema: esgf_dashboard; Owner: dbsuper; Tablespace:
--

ALTER TABLE ONLY dashboard_queue ADD CONSTRAINT dashboard_queue_pkey PRIMARY KEY (id);


ALTER TABLE esgf_dashboard.dashboard_queue OWNER TO dbsuper;
--
-- Drop the functions used for updating the dashboard_queue;
--
drop function if exists store_dashboard_queue() CASCADE;
drop function if exists update_url();
drop function if exists update_url(integer);
drop LANGUAGE if exists plpgsql;

--
--Copy the rows of the access_logging table into the dashboard_queue table
--

insert into esgf_dashboard.dashboard_queue(id, url_path, remote_addr,user_id_hash, user_idp, service_type, success, duration, size, timestamp) select id, url, remote_addr, user_id_hash, user_idp, service_type, success, duration, data_size, date_fetched from esgf_node_manager.access_logging;

--
-- Function to update the urls in the dashboard_queue table
--
CREATE LANGUAGE plpgsql;
create function update_url(integer)
returns integer as'
declare
i alias for $1;
j integer:=28+i;
begin
UPDATE esgf_dashboard.dashboard_queue SET url_path = substr(url_path,
j) where url_path like ''http%'';
return 0;
end;
'language 'plpgsql';

--
-- Insert into the dashboard_queue_table a new row stored in the access_logging table
--

CREATE FUNCTION store_dashboard_queue() RETURNS trigger AS
$store_new_entry$
declare
url_http varchar;
BEGIN
-- Update dashboard_queue table
insert into esgf_dashboard.dashboard_queue(id, url_path, remote_addr,
user_id_hash, user_idp, service_type, success, duration, size,
timestamp)values(NEW.id, NEW.url, NEW.remote_addr, NEW.user_id_hash,
NEW.user_idp, NEW.service_type, NEW.success, NEW.duration,
NEW.data_size, NEW.date_fetched);
url_http:=NEW.url;
if strpos(url_http,'http')<>0 then
update esgf_dashboard.dashboard_queue set url_path=subquery.url_res
FROM (select file.url as url_res from public.file_version as file,
esgf_dashboard.dashboard_queue as log where log.url_path like '%'||file.url
and log.url_path=url_http) as subquery where url_path=url_http;
end if;
raise notice 'insert into esgf_dashboard.dashboard_queue(id, url_path,
remote_addr, user_id_hash, user_idp, service_type, success, duration, size,
timestamp) values (%, ''%'', ''%'', ''%'',''%'',''%'', ''%'', %, %, %);', NEW.id,
NEW.url, NEW.remote_addr, NEW.user_id_hash, NEW.user_idp,
NEW.service_type, NEW.success, NEW.duration, NEW.data_size,
NEW.date_fetched;
RETURN NEW;
END
$store_new_entry$ LANGUAGE plpgsql;
CREATE TRIGGER store_new_entry
AFTER INSERT OR UPDATE OR DELETE ON
esgf_node_manager.access_logging
FOR EACH ROW EXECUTE PROCEDURE store_dashboard_queue();
