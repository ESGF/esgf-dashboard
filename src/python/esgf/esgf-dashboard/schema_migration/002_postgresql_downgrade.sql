SET statement_timeout = 0;
SET client_encoding = 'UTF8';
SET check_function_bodies = false;
SET client_min_messages = warning;
SET escape_string_warning = off;

SET search_path = esgf_dashboard, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

drop table esgf_dashboard.hasfeed ;

drop table esgf_dashboard.rssfeed ;

drop table esgf_dashboard.cpu_metrics ;

alter table esgf_dashboard.host drop regusers;
alter table esgf_dashboard.host drop nodetype;
alter table esgf_dashboard.host drop defaultpeer;
alter table esgf_dashboard.host drop suppemail;
alter table esgf_dashboard.host drop downloaddata;
alter table esgf_dashboard.host drop downloaddatacount;

