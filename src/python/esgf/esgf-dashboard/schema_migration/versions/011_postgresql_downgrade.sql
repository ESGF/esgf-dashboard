SET search_path = esgf_dashboard, pg_catalog;

drop trigger if exists store_new_entry on esgf_node_manager.access_logging;
drop trigger if exists store_update_entry on esgf_node_manager.access_logging;
drop trigger if exists store_delete_entry on esgf_node_manager.access_logging;
drop function if exists delete_dashboard_queue() CASCADE;
drop function if exists update_dashboard_queue() CASCADE;
drop function if exists store_dashboard_queue() CASCADE;

/* DASHBOARD_QUEUE DROP COLUMN */
ALTER TABLE esgf_dashboard.dashboard_queue DROP COLUMN user_id RESTRICT;

--
-- Insert into the dashboard_queue_table a new row stored in the access_logging table without user_id field
--
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

drop function if exists table_exists_userid() CASCADE;

/* REGISTRY DROP TABLE */
SET search_path = public, pg_catalog;
