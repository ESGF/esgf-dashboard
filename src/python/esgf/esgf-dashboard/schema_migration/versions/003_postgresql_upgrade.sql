--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET check_function_bodies = false;
SET client_min_messages = warning;
SET escape_string_warning = off;

SET search_path = esgf_dashboard, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

create table esgf_dashboard.data_download_metrics_dw (
	id serial,
	url character varying(1024),
	year smallint,
	month smallint,
	day smallint,
	project character varying(512),
	experiment  character varying(512),
	model  character varying(512),
	service_type smallint,
	remote_add  character varying(128),
	user_idp character varying(128),
	user_idhash  character varying(128),
	size bigint,
	success smallint,
	duration integer
);

create table esgf_dashboard.reconciliation_process (
	lastprocessed_id bigint,
	time_stamp timestamp without time zone DEFAULT now() NOT NULL
);


ALTER TABLE esgf_dashboard.data_download_metrics_dw OWNER TO dbsuper;

ALTER TABLE esgf_dashboard.reconciliation_process OWNER TO dbsuper;

insert INTO esgf_dashboard.reconciliation_process(lastprocessed_id,time_stamp) values(0,now());


SET search_path = public, pg_catalog;
