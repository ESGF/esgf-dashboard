DROP PROCEDURE IF EXISTS `avgHostActivity`;
DELIMITER $$
CREATE DEFINER=`root`@`localhost` PROCEDURE `avgHostActivity`(IN project_id INTEGER, IN host_id INTEGER, IN start_date DATETIME, IN end_date DATETIME)
BEGIN
        IF project_id IS NULL THEN
             SELECT AVG(percentage) as average FROM (
                SELECT 100*
                (
                        SELECT COUNT(*)
                        FROM service_status
                        WHERE idServiceInstance=s.id AND status=1 AND timestamp BETWEEN start_date AND end_date
                ) / (
                        SELECT COUNT(*)
                        FROM service_status
                        WHERE idServiceInstance=s.id AND timestamp BETWEEN start_date AND end_date
                )  AS percentage
                FROM service_instance s INNER JOIN uses u ON u.idServiceInstance=s.id
                WHERE s.idHost=host_id AND u.endDate IS NULL) t;
        ELSE
                SELECT AVG(percentage) as average FROM (
                SELECT 100*
                (
                        SELECT COUNT(*)
                        FROM service_status
                        WHERE idServiceInstance=s.id AND status=1 AND timestamp BETWEEN start_date AND end_date
                ) / (
                        SELECT COUNT(*)
                        FROM service_status
                        WHERE idServiceInstance=s.id AND timestamp BETWEEN start_date AND end_date
                )  AS percentage
                FROM service_instance s INNER JOIN uses u ON u.idServiceInstance=s.id
                WHERE s.idHost=host_id AND u.idProject=project_id AND u.endDate IS NULL) t;
        END IF;
END
$$