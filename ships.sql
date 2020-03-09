DROP TABLE IF EXISTS ships;
CREATE TABLE ships (
    id           INT NOT NULL AUTO_INCREMENT,
    length       INT NOT NULL,
    crew         INT NOT NULL,
    affiliation  VARCHAR(255) NOT NULL,
    category     VARCHAR(255) NOT NULL,
    manufacturer VARCHAR(255) NOT NULL,
    model        VARCHAR(255) NOT NULL,
    ship_class   VARCHAR(255) NOT NULL,
    PRIMARY KEY (id)
) CHARSET=utf8 COLLATE=utf8_unicode_ci;

DROP TABLE IF EXISTS ship_roles;
CREATE TABLE ship_roles (
    id   INT NOT NULL,
    role VARCHAR(255) NOT NULL,
    PRIMARY KEY (id, role)
) CHARSET=utf8 COLLATE=utf8_unicode_ci;

INSERT INTO ships (id, length, crew, affiliation, category, manufacturer, model, ship_class) VALUES
(1, 13, 1, "Rebel Alliance", "Starfighters", "Incom Corporation", "T-65 X-Wing", "Starfighter"),
(2, 7, 1, "Empire", "Starfighters", "Sienar Fleet Systems", "TIE/LN Starfighter", "Starfighter"),
(3, 8, 1, "Empire", "Starfighters", "Sienar Fleet Systems", "TIE/SA Bomber", "Space/Planetary Bomber"),
(4, 16, 1, "Rebel Alliance", "Starfighters", "Koensayr Manufacturing", "BTL-A4 Y-Wing", "Assault Starfighter"),
(5, 10, 1, "Rebel Alliance", "Starfighters", "Alliance Underground Engineering", "RZ-1 A-Wing Interceptor", "Starfighter"),
(6, 9, 1, "Empire", "Starfighters", "Sienar Fleet Systems", "TIE/D Defender", "Starfighter"),
(7, 10, 1, "Empire", "Starfighters", "Sienar Fleet Systems", "TIE/IN Interceptor", "Starfighter"),
(8, 1200, 5402, "Rebel Alliance", "Capital Ships", "Mon Calamari Shipyards", "MC80 Liberty Type Star Cruiser", "Star Cruiser"),
(9, 1600, 37085, "Empire", "Capital Ships", "Kuat Drive Yards", "Imperial I-class Star Destroyer", "Star Destroyer"),
(10, 1600, 37050, "Empire", "Capital Ships", "Kuat Drive Yards", "Imperial II-class Star Destroyer", "Star Destroyer"),
(11, 300, 850, "Rebel Alliance", "Escort Ships", "Kuat Drive Yards", "EF76 Nebulon-B Escort Frigate", "Frigate"),
(12, 150, 165, "Rebel Alliance", "Escort Ships", "Corellian Engineering Corporation", "CR90 Corvette", "Corvette"),
(13, 35, 2, "Rebel Alliance", "Transports", "Corellian Engineering Corporation", "YT-1300 Light Freighter", "Light Freighter"),
(14, 20, 2, "Empire", "Transports", "Sienar Fleet Systems", "Lambda-Class T-4a Shuttle", "Armed Government Transport Shuttle"),
(15, 90, 7, "Rebel Alliance", "Transports", "Gallofree Yards", "GR-75 Medium Transport", "Medium Transport");

INSERT INTO ship_roles (id, role) VALUES
(1, "Space Superiority Starfighter"),
(1, "Escort"),
(2, "Space Superiority Starfighter"),
(3, "Bomber"),
(4, "Starfighter"),
(4, "Bomber"),
(4, "Escort"),
(4, "Scout"),
(4, "Courier"),
(4, "Shuttle"),
(5, "Interceptor"),
(6, "Space Superiority Fighter"),
(6, "Interceptor"),
(6, "Light Bomber"),
(7, "Space Superiority Fighter"),
(7, "Interceptor"),
(8, "Carrier"),
(8, "Destroyer"),
(9, "Destroyer"),
(9, "Carrier"),
(9, "Military Transport"),
(9, "Command Ship"),
(10, "Destroyer"),
(10, "Carrier"),
(10, "Military Transport"),
(10, "Command Ship"),
(11, "Escort"),
(11, "Hospital Ship"),
(12, "Escort"),
(12, "Transport"),
(13, "Transport"),
(13, "Freighter"),
(13, "Smuggling Vessel"),
(14, "Personnel Shuttle"),
(14, "Troop Transport"),
(14, "Escape Vessel"),
(15, "Transport"),
(15, "Fuel Tanker"),
(15, "Salvage Ship"),
(15, "Landing Craft"),
(15, "Hospital Ship");
