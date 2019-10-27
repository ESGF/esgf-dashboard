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
DROP TABLE IF EXISTS esgf_dashboard.downloads_info CASCADE;
DROP TABLE IF EXISTS esgf_dashboard.registry_collector CASCADE;

CREATE TABLE downloads_info (
    id bigserial PRIMARY KEY,                           -- unique id
    id_acc integer NOT NULL,                            -- remote access_logging table id                       
    url_path character varying NOT NULL,                -- path of the downloaded file
    user_id_hash character varying,                     -- hash code of the user id
    user_idp character varying,                         -- user identity provider
    service_type character varying,                     -- download service type
    success boolean,                                    -- outcome of the download operation
    duration double precision,                          -- duration of the download operation
    size bigint DEFAULT (-1),                           -- file dimensions
    "timestamp" double precision NOT NULL,              -- download time instant
    country_user character varying,                     -- country_user depends by remote_addr
    hostname character varying NOT NULL,                -- remote datanode 
    processed smallint DEFAULT 0 NOT NULL               -- dashboard flag
);

ALTER TABLE esgf_dashboard.downloads_info OWNER TO dbsuper;

CREATE TABLE registry_collector (
    id bigserial PRIMARY KEY,                           -- unique id
    datanode character varying NOT NULL,                -- remote datanode
    indexnode character varying NOT NULL,               -- remote indexnode
    port integer NOT NULL,                              -- local port associated with the replica indexnode
    "timestamp" double precision NOT NULL,              -- download time instant
    success boolean                                    -- outcome of the download operation
);

ALTER TABLE esgf_dashboard.registry_collector OWNER TO dbsuper;

insert into registry_collector (datanode, indexnode, port, timestamp, success) values ('esg-node.cmcc.it', 'esgf-data.dkrz.de', 8989, 0, 't');
insert into registry_collector (datanode, indexnode, port, timestamp, success) values ('aim3.llnl.gov', 'esgf-node.llnl.gov', 8985, 0, 't');
insert into registry_collector (datanode, indexnode, port, timestamp, success) values ('esgdata.gfdl.noaa.gov', 'esgdata.gfdl.noaa.gov', 8986, 0, 't');
insert into registry_collector (datanode, indexnode, port, timestamp, success) values ('esgf.nccs.nasa.gov', 'esgf.nccs.nasa.gov', 8987, 0, 't');
insert into registry_collector (datanode, indexnode, port, timestamp, success) values ('esgf.nci.org.au', 'esgf.nci.org.au', 8988, 0, 't');
insert into registry_collector (datanode, indexnode, port, timestamp, success) values ('esgf1.dkrz.de', 'esgf-data.dkrz.de', 8989, 0, 't');


SET search_path = public, pg_catalog; 
