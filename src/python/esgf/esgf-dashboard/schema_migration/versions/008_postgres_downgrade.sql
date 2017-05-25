SET search_path = esgf_dashboard, pg_catalog;
--
-- Drop the dashboard_queue table
--

drop table if exists esgf_dashboard.dashboard_queue;

--
-- Drop the triggers and functions used for updating the dashboard_queue;
--
drop trigger if exists store_new_entry ON esgf_node_manager.access_logging CASCADE;
drop trigger if exists store_update_entry ON esgf_node_manager.access_logging CASCADE;
drop trigger if exists store_delete_entry ON esgf_node_manager.access_logging CASCADE;
drop function if exists store_dashboard_queue() CASCADE;
drop function if exists update_dashboard_queue() CASCADE;
drop function if exists delete_dashboard_queue() CASCADE;
drop language if exists plpgsql;
SET search_path = public, pg_catalog;
