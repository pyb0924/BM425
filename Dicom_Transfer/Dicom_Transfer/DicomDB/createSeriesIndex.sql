CREATE TABLE (
    patientName VARCHAR(64) NOT NULL,
    patientID VARCHAR(64) NOT NULL,
    studyID VARCHAR(16) NOT NULL,
    studyInstanceUID VARCHAR(64) NOT NULL,
    seriesDescription VARCHAR(64) NOT NULL,
    seriesInstanceUID VARCHAR(64) NOT NULL UNIQUE,
    seriesNumber SMALLINT NOT NULL,
    PRIMARY KEY (seriesInstanceUID)
);