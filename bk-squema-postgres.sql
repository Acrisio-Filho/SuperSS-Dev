--
-- PostgreSQL database dump
--

-- Dumped from database version 13.4 (Ubuntu 13.4-4.pgdg20.04+1)
-- Dumped by pg_dump version 13.3

-- Started on 2021-10-19 09:02:04

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

--
-- TOC entry 12 (class 2615 OID 23160)
-- Name: pangya; Type: SCHEMA; Schema: -; Owner: postgres
--

CREATE SCHEMA pangya;


ALTER SCHEMA pangya OWNER TO postgres;

--
-- TOC entry 458 (class 1255 OID 23161)
-- Name: addmsgoff(integer, integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.addmsgoff(_from_uid integer, _uid_to integer, _msg_in text) RETURNS void
    LANGUAGE plpgsql
    AS $$	
BEGIN

	INSERT INTO pangya.pangya_msg_user(
         uid, 
         uid_from, 
         valid, 
         msg, 
         reg_date)
         VALUES (
            _uid_to, 
            _from_uid, 
            1, 
            _msg_in, 
            now());

      UPDATE pangya.user_info
         SET 
            pang = Pang - 10
      WHERE UID = _from_uid;
END;
$$;


ALTER FUNCTION pangya.addmsgoff(_from_uid integer, _uid_to integer, _msg_in text) OWNER TO postgres;

--
-- TOC entry 459 (class 1255 OID 23162)
-- Name: checkdolfinipassword(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.checkdolfinipassword(_iduser integer) RETURNS TABLE("_OPT_" integer)
    LANGUAGE plpgsql
    AS $$	
BEGIN

	IF (
		SELECT uid
		FROM pangya.pangya_dolfini_locker
		WHERE uid = _IDUSER
	) IS NULL THEN
	BEGIN

		INSERT INTO pangya.pangya_dolfini_locker(uid, pang, locker)
		   VALUES (_IDUSER, 0, 0);

		RETURN QUERY SELECT 2 AS OPT;

	END;
	ELSIF 
	(
	   SELECT senha
	   FROM pangya.pangya_dolfini_locker
	   WHERE uid = _IDUSER
	) IS NULL THEN
		RETURN QUERY SELECT 2 AS OPT;
	ELSE 
		RETURN QUERY SELECT 76 AS OPT;
	END IF;
END;
$$;


ALTER FUNCTION pangya.checkdolfinipassword(_iduser integer) OWNER TO postgres;

--
-- TOC entry 461 (class 1255 OID 23163)
-- Name: checkweblink_key$impl(text, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya."checkweblink_key$impl"(_id_in text, _weblink_key text, OUT _returnvalue integer) RETURNS integer
    LANGUAGE plpgsql
    AS $$
DECLARE
	_RET int = -1;
BEGIN

	_returnvalue = null::int;

	_RET = COALESCE(
		(
			SELECT a.uid
			FROM pangya.pangya_weblink_key  AS a, pangya.account  AS b
			WHERE 
				b.ID = _id_in AND 
				b.UID = a.uid AND 
				a.key = _weblink_key AND 
				a.valid = 1
		), -1);

	IF _RET >= 0 THEN
		UPDATE pangya.pangya_weblink_key
			SET 
				valid = 0
		WHERE uid = _RET;
	END IF;

	_returnvalue = _RET;
END;
$$;


ALTER FUNCTION pangya."checkweblink_key$impl"(_id_in text, _weblink_key text, OUT _returnvalue integer) OWNER TO postgres;

--
-- TOC entry 462 (class 1255 OID 23164)
-- Name: evento1(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.evento1(_iduser integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
DECLARE
	_MSGID int;
BEGIN

	INSERT INTO pangya.pangya_gift_table(
		 UID, 
		 fromid, 
		 message, 
		 giftdate, 
		 Lida_YN, 
		 valid, 
		 Flag, 
		 Contador_Vista)
	 VALUES (
		_IDUSER, 
		'@SUPER', 
		'NICE FINISH TUTORIAL ROOKIE', 
		now(), 
		0, 
		1, 
		0, 
		0)
	RETURNING Msg_ID INTO _MSGID;

	perform pangya.ProcInsertItemNoEmail 
	 (0, 
	 _IDUSER, 
	 _MSGID, 
	 -1, 
	 x'1C000000', 
	 0, 
	 1, 
	 0);

	perform pangya.ProcInsertItemNoEmail 
	 (0, 
	 _IDUSER, 
	 _MSGID, 
	 -1, 
	 x'10000012', 
	 0, 
	 1, 
	 0);

	UPDATE pangya.account
	 SET 
		Event1 = 1
	WHERE UID = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.evento1(_iduser integer) OWNER TO postgres;

--
-- TOC entry 463 (class 1255 OID 23165)
-- Name: evento2(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.evento2(_iduser integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
DECLARE
	_MSGID int;
BEGIN

	INSERT INTO pangya.pangya_gift_table(
		UID, 
		fromid, 
		message, 
		giftdate, 
		Lida_YN, 
		valid, 
		Flag, 
		Contador_Vista)
	VALUES (
	_IDUSER, 
	'@SUPER', 
	'NICE FINISH TUTORIAL BEGINNER', 
	now(), 
	0, 
	1, 
	0, 
	0);

	SELECT max(Msg_ID) INTO _MSGID
	FROM pangya.pangya_gift_table;

	perform pangya.ProcInsertItemNoEmail 
	(0, 
	_IDUSER, 
	_MSGID, 
	-1, 
	x'18000027', 
	0, 
	10, 
	0);

	perform pangya.ProcInsertItemNoEmail 
	(0, 
	_IDUSER, 
	_MSGID, 
	-1, 
	x'1A000010', 
	0, 
	10000, 
	0);

	UPDATE pangya.account
		SET 
			Event2 = 1
	WHERE UID = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.evento2(_iduser integer) OWNER TO postgres;

--
-- TOC entry 471 (class 1255 OID 23166)
-- Name: flushtutorial(integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.flushtutorial(_iduser integer, _tipo integer, _valor integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN

	IF _TIPO = 0 THEN
	BEGIN

		UPDATE pangya.tutorial
		   SET 
			  Rookie = Rookie + _VALOR
		WHERE UID = _IDUSER;

		perform pangya.ProcDestribuiItemTutorial(_IDUSER, _VALOR, 0);

	END;
	ELSIF _TIPO = 1 THEN
	BEGIN

	   UPDATE pangya.tutorial
		  SET 
			 Rookie = Rookie + _VALOR
	   WHERE UID = _IDUSER;

	   UPDATE pangya.account
		  SET 
			 doTutorial = 1
	   WHERE UID = _IDUSER;

	   perform pangya.ProcDestribuiItemTutorial(_IDUSER, _VALOR, 0);

	   perform pangya.Evento1(_IDUSER);

	END;
	ELSIF _TIPO = 256 THEN
	DECLARE
		_TESTE int;
	BEGIN

		 UPDATE pangya.tutorial
			SET 
			   Beginner = Beginner + _VALOR
		 WHERE UID = _IDUSER;

		 perform pangya.ProcDestribuiItemTutorial(_IDUSER, _VALOR, 1);

		 SELECT Beginner INTO _TESTE
		 FROM pangya.tutorial
		 WHERE UID = _IDUSER;
	
		IF _TESTE = 16128 THEN

		   UPDATE pangya.account
			  SET 
				 doTutorial = 1
		   WHERE UID = _IDUSER;

		   perform pangya.Evento2(_IDUSER);

		END IF;
	END;
	END IF;
END;
$$;


ALTER FUNCTION pangya.flushtutorial(_iduser integer, _tipo integer, _valor integer) OWNER TO postgres;

--
-- TOC entry 485 (class 1255 OID 23167)
-- Name: funcconcertapartscharacter(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.funcconcertapartscharacter(_iduser integer, _chartype integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
DECLARE
	_CHAR_TYPE int;
BEGIN
	_CHAR_TYPE = (_CHARTYPE & 255);
	
	IF _CHAR_TYPE = 0 THEN
	BEGIN

		UPDATE pangya.pangya_character_information
			SET 
				parts_1 = 134218752
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_3 = 134235136
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_4 = 134243328
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_5 = 134251520
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_6 = 134259712
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_8 = 134276096
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_9 = 134284288
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

	END;
	ELSIF _CHAR_TYPE = 1 THEN
	BEGIN

		UPDATE pangya.pangya_character_information
			SET 
				parts_1 = 134480896
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_2 = 134489088
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_3 = 134497280
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_4 = 134505472
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_5 = 134513664
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_7 = 134530048
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_8 = 134538240
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

	END;
	ELSIF _CHAR_TYPE = 2 THEN
	BEGIN

		UPDATE pangya.pangya_character_information
			SET 
				parts_1 = 134743040
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_2 = 134751232
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_3 = 134759424
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_4 = 134767616
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_5 = 134775808
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_6 = 134784000
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_7 = 134792192
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_8 = 134800384
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

	END;
	ELSIF _CHAR_TYPE = 3 THEN
	BEGIN

		UPDATE pangya.pangya_character_information
			SET 
				parts_1 = 135005184
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_2 = 135013376
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_3 = 135021568
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_4 = 135029760
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_5 = 135037952
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_6 = 135046144
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_7 = 135054336
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_8 = 135062528
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

	END;
	ELSIF _CHAR_TYPE = 4 THEN
	BEGIN

		UPDATE pangya.pangya_character_information
			SET 
				parts_1 = 135267328
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_2 = 135275520
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_3 = 135283712
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_4 = 135291904
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_5 = 135300096
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_6 = 135308288
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

	END;
	ELSIF _CHAR_TYPE = 5 THEN
	BEGIN

		UPDATE pangya.pangya_character_information
			SET 
				parts_1 = 135529472
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_2 = 135537664
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_3 = 135545856
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_4 = 135554048
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_5 = 135562240
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_6 = 135570432
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_7 = 135578624
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_8 = 135586816
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

	END;
	ELSIF _CHAR_TYPE = 6 THEN
	BEGIN

		UPDATE pangya.pangya_character_information
			SET 
				parts_1 = 135791616
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_2 = 135799808
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_3 = 135808000
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_4 = 135816192
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_5 = 135824384
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_6 = 135832576
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_7 = 135840768
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

	END;
	ELSIF _CHAR_TYPE = 7 THEN
	BEGIN

		UPDATE pangya.pangya_character_information
			SET 
				parts_1 = 136053760
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_2 = 136061952
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_3 = 136070144
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_4 = 136078336
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_5 = 136086528
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_6 = 136094720
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_7 = 136102912
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_8 = 136111104
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_9 = 136119296
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

	END;
	ELSIF _CHAR_TYPE = 8 THEN
	BEGIN

		UPDATE pangya.pangya_character_information
			SET 
				parts_1 = 136315904
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_2 = 136324096
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_3 = 136332288
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_4 = 136340480
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_5 = 136348672
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_6 = 136356864
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_7 = 136365056
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_8 = 136373248
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

	END;
	ELSIF _CHAR_TYPE = 9 THEN
	BEGIN

		UPDATE pangya.pangya_character_information
			SET 
				parts_1 = 136578048
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_2 = 136586240
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_3 = 136594432
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_4 = 136602624
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_5 = 136610816
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_6 = 136619008
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_7 = 136627200
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_8 = 136635392
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

	END;
	ELSIF _CHAR_TYPE = 10 THEN

		UPDATE pangya.pangya_character_information
			SET 
				parts_1 = 136840192
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_2 = 136848384
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_3 = 136856576
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_4 = 136864768
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_5 = 136872960
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_6 = 136881152
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_7 = 136889344
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;

		UPDATE pangya.pangya_character_information
			SET 
				parts_8 = 136897536
		WHERE UID = _IDUSER AND typeid = _CHARTYPE;
	END IF;
END;
$$;


ALTER FUNCTION pangya.funcconcertapartscharacter(_iduser integer, _chartype integer) OWNER TO postgres;

--
-- TOC entry 486 (class 1255 OID 23168)
-- Name: funcinsertpartscharacter(integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.funcinsertpartscharacter(_iduser integer, _chartype integer, _shirts integer, _hairs integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
DECLARE
	_CHAR_TYPE int;
BEGIN
	_CHAR_TYPE = (_CHARTYPE & 255);

    IF _CHAR_TYPE = 0 THEN
    BEGIN

        INSERT INTO pangya.pangya_character_information(
            UID, 
            typeid, 
            parts_1, 
            parts_2, 
            parts_3, 
            parts_4, 
            parts_5, 
            parts_6, 
            parts_7, 
            parts_8, 
            parts_9, 
            parts_10, 
            parts_11, 
            parts_12, 
            parts_13, 
            parts_14, 
            parts_15, 
            parts_16, 
            parts_17, 
            parts_18, 
            parts_19, 
            parts_20, 
            parts_21, 
            parts_22, 
            parts_23, 
            parts_24, 
            default_hair, 
            default_shirts, 
            gift_flag, 
            PCL0, 
            PCL1, 
            PCL2, 
            PCL3, 
            PCL4, 
            Purchase, 
            auxparts_1, 
            auxparts_2, 
            auxparts_3, 
            auxparts_4, 
            auxparts_5, 
            Mastery)
        VALUES (
                _IDUSER, 
                _CHARTYPE, 
                134218752, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0);

        UPDATE pangya.pangya_character_information
            SET 
                parts_3 = 134235136
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
                parts_4 = 134243328
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
                parts_5 = 134251520
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
                parts_6 = 134259712
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
                parts_8 = 134276096
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
                parts_9 = 134284288
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

    END;
    ELSIF _CHAR_TYPE = 1 THEN
    BEGIN

        INSERT INTO pangya.pangya_character_information(
            UID, 
            typeid, 
            parts_1, 
            parts_2, 
            parts_3, 
            parts_4, 
            parts_5, 
            parts_6, 
            parts_7, 
            parts_8, 
            parts_9, 
            parts_10, 
            parts_11, 
            parts_12, 
            parts_13, 
            parts_14, 
            parts_15, 
            parts_16, 
            parts_17, 
            parts_18, 
            parts_19, 
            parts_20, 
            parts_21, 
            parts_22, 
            parts_23, 
            parts_24, 
            default_hair, 
            default_shirts, 
            gift_flag, 
            PCL0, 
            PCL1, 
            PCL2, 
            PCL3, 
            PCL4, 
            Purchase, 
            auxparts_1, 
            auxparts_2, 
            auxparts_3, 
            auxparts_4, 
            auxparts_5, 
            Mastery)
        VALUES (
                _IDUSER, 
                _CHARTYPE, 
                134480896, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0);

        UPDATE pangya.pangya_character_information
            SET 
                parts_2 = 134489088
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
                parts_3 = 134497280
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
                parts_4 = 134505472
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
                parts_5 = 134513664
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
                parts_7 = 134530048
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
                parts_8 = 134538240
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

    END;
    ELSIF _CHAR_TYPE = 2 THEN
    BEGIN

        INSERT INTO pangya.pangya_character_information(
            UID, 
            typeid, 
            parts_1, 
            parts_2, 
            parts_3, 
            parts_4, 
            parts_5, 
            parts_6, 
            parts_7, 
            parts_8, 
            parts_9, 
            parts_10, 
            parts_11, 
            parts_12, 
            parts_13, 
            parts_14, 
            parts_15, 
            parts_16, 
            parts_17, 
            parts_18, 
            parts_19, 
            parts_20, 
            parts_21, 
            parts_22, 
            parts_23, 
            parts_24, 
            default_hair, 
            default_shirts, 
            gift_flag, 
            PCL0, 
            PCL1, 
            PCL2, 
            PCL3, 
            PCL4, 
            Purchase, 
            auxparts_1, 
            auxparts_2, 
            auxparts_3, 
            auxparts_4, 
            auxparts_5, 
            Mastery)
        VALUES (
            _IDUSER, 
            _CHARTYPE, 
            134743040, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0);

        UPDATE pangya.pangya_character_information
            SET 
            parts_2 = 134751232
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
            parts_3 = 134759424
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
            parts_4 = 134767616
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
            parts_5 = 134775808
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
            parts_6 = 134784000
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
            parts_7 = 134792192
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
            parts_8 = 134800384
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

    END;
    ELSIF _CHAR_TYPE = 3 THEN
    BEGIN

        INSERT INTO pangya.pangya_character_information(
            UID, 
            typeid, 
            parts_1, 
            parts_2, 
            parts_3, 
            parts_4, 
            parts_5, 
            parts_6, 
            parts_7, 
            parts_8, 
            parts_9, 
            parts_10, 
            parts_11, 
            parts_12, 
            parts_13, 
            parts_14, 
            parts_15, 
            parts_16, 
            parts_17, 
            parts_18, 
            parts_19, 
            parts_20, 
            parts_21, 
            parts_22, 
            parts_23, 
            parts_24, 
            default_hair, 
            default_shirts, 
            gift_flag, 
            PCL0, 
            PCL1, 
            PCL2, 
            PCL3, 
            PCL4, 
            Purchase, 
            auxparts_1, 
            auxparts_2, 
            auxparts_3, 
            auxparts_4, 
            auxparts_5, 
            Mastery)
        VALUES (
            _IDUSER, 
            _CHARTYPE, 
            135005184, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0);

        UPDATE pangya.pangya_character_information
        SET 
            parts_2 = 135013376
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
        SET 
            parts_3 = 135021568
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
        SET 
            parts_4 = 135029760
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
        SET 
            parts_5 = 135037952
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
        SET 
            parts_6 = 135046144
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
        SET 
            parts_7 = 135054336
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;


        UPDATE pangya.pangya_character_information
        SET 
            parts_8 = 135062528
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

    END;
    ELSIF _CHAR_TYPE = 4 THEN
    BEGIN

        INSERT INTO pangya.pangya_character_information(
            UID, 
            typeid, 
            parts_1, 
            parts_2, 
            parts_3, 
            parts_4, 
            parts_5, 
            parts_6, 
            parts_7, 
            parts_8, 
            parts_9, 
            parts_10, 
            parts_11, 
            parts_12, 
            parts_13, 
            parts_14, 
            parts_15, 
            parts_16, 
            parts_17, 
            parts_18, 
            parts_19, 
            parts_20, 
            parts_21, 
            parts_22, 
            parts_23, 
            parts_24, 
            default_hair, 
            default_shirts, 
            gift_flag, 
            PCL0, 
            PCL1, 
            PCL2, 
            PCL3, 
            PCL4, 
            Purchase, 
            auxparts_1, 
            auxparts_2, 
            auxparts_3, 
            auxparts_4, 
            auxparts_5, 
            Mastery)
        VALUES (
                _IDUSER, 
                _CHARTYPE, 
                135267328, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0);

        UPDATE pangya.pangya_character_information
            SET 
                parts_2 = 135275520
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
                parts_3 = 135283712
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
                parts_4 = 135291904
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
                parts_5 = 135300096
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
                parts_6 = 135308288
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

    END;
    ELSIF _CHAR_TYPE = 5 THEN
    BEGIN

        INSERT INTO pangya.pangya_character_information(
            UID, 
            typeid, 
            parts_1, 
            parts_2, 
            parts_3, 
            parts_4, 
            parts_5, 
            parts_6, 
            parts_7, 
            parts_8, 
            parts_9, 
            parts_10, 
            parts_11, 
            parts_12, 
            parts_13, 
            parts_14, 
            parts_15, 
            parts_16, 
            parts_17, 
            parts_18, 
            parts_19, 
            parts_20, 
            parts_21, 
            parts_22, 
            parts_23, 
            parts_24, 
            default_hair, 
            default_shirts, 
            gift_flag, 
            PCL0, 
            PCL1, 
            PCL2, 
            PCL3, 
            PCL4, 
            Purchase, 
            auxparts_1, 
            auxparts_2, 
            auxparts_3, 
            auxparts_4, 
            auxparts_5, 
            Mastery)
        VALUES (
                _IDUSER, 
                _CHARTYPE, 
                135529472, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0);

        UPDATE pangya.pangya_character_information
            SET 
                parts_2 = 135537664
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
                parts_3 = 135545856
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
                parts_4 = 135554048
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
                parts_5 = 135562240
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
                parts_6 = 135570432
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
                parts_7 = 135578624
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
                parts_8 = 135586816
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

    END;
    ELSIF _CHAR_TYPE = 6 THEN
    BEGIN

        INSERT INTO pangya.pangya_character_information(
            UID, 
            typeid, 
            parts_1, 
            parts_2, 
            parts_3, 
            parts_4, 
            parts_5, 
            parts_6, 
            parts_7, 
            parts_8, 
            parts_9, 
            parts_10, 
            parts_11, 
            parts_12, 
            parts_13, 
            parts_14, 
            parts_15, 
            parts_16, 
            parts_17, 
            parts_18, 
            parts_19, 
            parts_20, 
            parts_21, 
            parts_22, 
            parts_23, 
            parts_24, 
            default_hair, 
            default_shirts, 
            gift_flag, 
            PCL0, 
            PCL1, 
            PCL2, 
            PCL3, 
            PCL4, 
            Purchase, 
            auxparts_1, 
            auxparts_2, 
            auxparts_3, 
            auxparts_4, 
            auxparts_5, 
            Mastery)
        VALUES (
            _IDUSER, 
            _CHARTYPE, 
            135791616, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0);

        UPDATE pangya.pangya_character_information
            SET 
            parts_2 = 135799808
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
            parts_3 = 135808000
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
            parts_4 = 135816192
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
            parts_5 = 135824384
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
            parts_6 = 135832576
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
            parts_7 = 135840768
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

    END;
    ELSIF _CHAR_TYPE = 7 THEN
    BEGIN

        INSERT INTO pangya.pangya_character_information(
            UID, 
            typeid, 
            parts_1, 
            parts_2, 
            parts_3, 
            parts_4, 
            parts_5, 
            parts_6, 
            parts_7, 
            parts_8, 
            parts_9, 
            parts_10, 
            parts_11, 
            parts_12, 
            parts_13, 
            parts_14, 
            parts_15, 
            parts_16, 
            parts_17, 
            parts_18, 
            parts_19, 
            parts_20, 
            parts_21, 
            parts_22, 
            parts_23, 
            parts_24, 
            default_hair, 
            default_shirts, 
            gift_flag, 
            PCL0, 
            PCL1, 
            PCL2, 
            PCL3, 
            PCL4, 
            Purchase, 
            auxparts_1, 
            auxparts_2, 
            auxparts_3, 
            auxparts_4, 
            auxparts_5, 
            Mastery)
        VALUES (
            _IDUSER, 
            _CHARTYPE, 
            136053760, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0);

        UPDATE pangya.pangya_character_information
        SET 
            parts_2 = 136061952
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
        SET 
            parts_3 = 136070144
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
        SET 
            parts_4 = 136078336
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
        SET 
            parts_5 = 136086528
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
        SET 
            parts_6 = 136094720
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
        SET 
            parts_7 = 136102912
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
        SET 
            parts_8 = 136111104
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
        SET 
            parts_9 = 136119296
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

    END;
    ELSIF _CHAR_TYPE = 8 THEN
    BEGIN

        INSERT INTO pangya.pangya_character_information(
            UID, 
            typeid, 
            parts_1, 
            parts_2, 
            parts_3, 
            parts_4, 
            parts_5, 
            parts_6, 
            parts_7, 
            parts_8, 
            parts_9, 
            parts_10, 
            parts_11, 
            parts_12, 
            parts_13, 
            parts_14, 
            parts_15, 
            parts_16, 
            parts_17, 
            parts_18, 
            parts_19, 
            parts_20, 
            parts_21, 
            parts_22, 
            parts_23, 
            parts_24, 
            default_hair, 
            default_shirts, 
            gift_flag, 
            PCL0, 
            PCL1, 
            PCL2, 
            PCL3, 
            PCL4, 
            Purchase, 
            auxparts_1, 
            auxparts_2, 
            auxparts_3, 
            auxparts_4, 
            auxparts_5, 
            Mastery)
        VALUES (
                _IDUSER, 
                _CHARTYPE, 
                136315904, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0);

    UPDATE pangya.pangya_character_information
        SET 
            parts_2 = 136324096
    WHERE UID = _IDUSER AND typeid = _CHARTYPE;

    UPDATE pangya.pangya_character_information
        SET 
            parts_3 = 136332288
    WHERE UID = _IDUSER AND typeid = _CHARTYPE;

    UPDATE pangya.pangya_character_information
        SET 
            parts_4 = 136340480
    WHERE UID = _IDUSER AND typeid = _CHARTYPE;

    UPDATE pangya.pangya_character_information
        SET 
            parts_5 = 136348672
    WHERE UID = _IDUSER AND typeid = _CHARTYPE;

    UPDATE pangya.pangya_character_information
        SET 
            parts_6 = 136356864
    WHERE UID = _IDUSER AND typeid = _CHARTYPE;

    UPDATE pangya.pangya_character_information
        SET 
            parts_7 = 136365056
    WHERE UID = _IDUSER AND typeid = _CHARTYPE;

    UPDATE pangya.pangya_character_information
        SET 
            parts_8 = 136373248
    WHERE UID = _IDUSER AND typeid = _CHARTYPE;

    END;
    ELSIF _CHAR_TYPE = 9 THEN
    BEGIN

        INSERT INTO pangya.pangya_character_information(
            UID, 
            typeid, 
            parts_1, 
            parts_2, 
            parts_3, 
            parts_4, 
            parts_5, 
            parts_6, 
            parts_7, 
            parts_8, 
            parts_9, 
            parts_10, 
            parts_11, 
            parts_12, 
            parts_13, 
            parts_14, 
            parts_15, 
            parts_16, 
            parts_17, 
            parts_18, 
            parts_19, 
            parts_20, 
            parts_21, 
            parts_22, 
            parts_23, 
            parts_24, 
            default_hair, 
            default_shirts, 
            gift_flag, 
            PCL0, 
            PCL1, 
            PCL2, 
            PCL3, 
            PCL4, 
            Purchase, 
            auxparts_1, 
            auxparts_2, 
            auxparts_3, 
            auxparts_4, 
            auxparts_5, 
            Mastery)
        VALUES (
                _IDUSER, 
                _CHARTYPE, 
                136578048, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0);

        UPDATE pangya.pangya_character_information
            SET 
                parts_2 = 136586240
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
                parts_3 = 136594432
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
                parts_4 = 136602624
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
                parts_5 = 136610816
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
                parts_6 = 136619008
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
                parts_7 = 136627200
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
                parts_8 = 136635392
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

    END;
    ELSIF _CHAR_TYPE = 10 THEN
    BEGIN

        INSERT INTO pangya.pangya_character_information(
            UID, 
            typeid, 
            parts_1, 
            parts_2, 
            parts_3, 
            parts_4, 
            parts_5, 
            parts_6, 
            parts_7, 
            parts_8, 
            parts_9, 
            parts_10, 
            parts_11, 
            parts_12, 
            parts_13, 
            parts_14, 
            parts_15, 
            parts_16, 
            parts_17, 
            parts_18, 
            parts_19, 
            parts_20, 
            parts_21, 
            parts_22, 
            parts_23, 
            parts_24, 
            default_hair, 
            default_shirts, 
            gift_flag, 
            PCL0, 
            PCL1, 
            PCL2, 
            PCL3, 
            PCL4, 
            Purchase, 
            auxparts_1, 
            auxparts_2, 
            auxparts_3, 
            auxparts_4, 
            auxparts_5, 
            Mastery)
        VALUES (
            _IDUSER, 
            _CHARTYPE, 
            136840192, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0);

        UPDATE pangya.pangya_character_information
            SET 
            parts_2 = 136848384
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
            parts_3 = 136856576
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
            parts_4 = 136864768
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
            parts_5 = 136872960
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
            parts_6 = 136881152
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
            parts_7 = 136889344
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
            parts_8 = 136897536
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

    END;
    ELSIF _CHAR_TYPE = 11 THEN
    BEGIN

        INSERT INTO pangya.pangya_character_information(
            UID, 
            typeid, 
            parts_1, 
            parts_2, 
            parts_3, 
            parts_4, 
            parts_5, 
            parts_6, 
            parts_7, 
            parts_8, 
            parts_9, 
            parts_10, 
            parts_11, 
            parts_12, 
            parts_13, 
            parts_14, 
            parts_15, 
            parts_16, 
            parts_17, 
            parts_18, 
            parts_19, 
            parts_20, 
            parts_21, 
            parts_22, 
            parts_23, 
            parts_24, 
            default_hair, 
            default_shirts, 
            gift_flag, 
            PCL0, 
            PCL1, 
            PCL2, 
            PCL3, 
            PCL4, 
            Purchase, 
            auxparts_1, 
            auxparts_2, 
            auxparts_3, 
            auxparts_4, 
            auxparts_5, 
            Mastery)
        VALUES (
            _IDUSER, 
            _CHARTYPE, 
            137102336, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0, 
            0);

        UPDATE pangya.pangya_character_information
        SET 
            parts_2 = 137110528
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
        SET 
            parts_3 = 137118720
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
        SET 
            parts_4 = 137126912
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
        SET 
            parts_5 = 137135104
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
        SET 
            parts_6 = 137143296
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
        SET 
            parts_7 = 137151488
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
        SET 
            parts_8 = 137159680
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

    END;
    ELSIF _CHAR_TYPE = 12 THEN
    BEGIN

        INSERT INTO pangya.pangya_character_information(
            UID, 
            typeid, 
            parts_1, 
            parts_2, 
            parts_3, 
            parts_4, 
            parts_5, 
            parts_6, 
            parts_7, 
            parts_8, 
            parts_9, 
            parts_10, 
            parts_11, 
            parts_12, 
            parts_13, 
            parts_14, 
            parts_15, 
            parts_16, 
            parts_17, 
            parts_18, 
            parts_19, 
            parts_20, 
            parts_21, 
            parts_22, 
            parts_23, 
            parts_24, 
            default_hair, 
            default_shirts, 
            gift_flag, 
            PCL0, 
            PCL1, 
            PCL2, 
            PCL3, 
            PCL4, 
            Purchase, 
            auxparts_1, 
            auxparts_2, 
            auxparts_3, 
            auxparts_4, 
            auxparts_5, 
            Mastery)
        VALUES (
                _IDUSER, 
                _CHARTYPE, 
                137364480, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0);

    UPDATE pangya.pangya_character_information
        SET 
            parts_2 = 137372672
    WHERE UID = _IDUSER AND typeid = _CHARTYPE;

    UPDATE pangya.pangya_character_information
        SET 
            parts_3 = 137380864
    WHERE UID = _IDUSER AND typeid = _CHARTYPE;

    UPDATE pangya.pangya_character_information
        SET 
            parts_4 = 137389056
    WHERE UID = _IDUSER AND typeid = _CHARTYPE;

    UPDATE pangya.pangya_character_information
        SET 
            parts_5 = 137397248
    WHERE UID = _IDUSER AND typeid = _CHARTYPE;

    UPDATE pangya.pangya_character_information
        SET 
            parts_6 = 137405440
    WHERE UID = _IDUSER AND typeid = _CHARTYPE;

    UPDATE pangya.pangya_character_information
        SET 
            parts_7 = 137413632
    WHERE UID = _IDUSER AND typeid = _CHARTYPE;

    UPDATE pangya.pangya_character_information
        SET 
            parts_8 = 137421824
    WHERE UID = _IDUSER AND typeid = _CHARTYPE;

    END;
    ELSIF _CHAR_TYPE = 14 THEN

        INSERT INTO pangya.pangya_character_information(
            UID, 
            typeid, 
            parts_1, 
            parts_2, 
            parts_3, 
            parts_4, 
            parts_5, 
            parts_6, 
            parts_7, 
            parts_8, 
            parts_9, 
            parts_10, 
            parts_11, 
            parts_12, 
            parts_13, 
            parts_14, 
            parts_15, 
            parts_16, 
            parts_17, 
            parts_18, 
            parts_19, 
            parts_20, 
            parts_21, 
            parts_22, 
            parts_23, 
            parts_24, 
            default_hair, 
            default_shirts, 
            gift_flag, 
            PCL0, 
            PCL1, 
            PCL2, 
            PCL3, 
            PCL4, 
            Purchase, 
            auxparts_1, 
            auxparts_2, 
            auxparts_3, 
            auxparts_4, 
            auxparts_5, 
            Mastery)
        VALUES (
                _IDUSER, 
                _CHARTYPE, 
                137888768, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0, 
                0);

        UPDATE pangya.pangya_character_information
            SET 
                parts_2 = 137896960
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
                parts_3 = 137905152
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
                parts_4 = 137913344
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
                parts_5 = 137921536
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
                parts_6 = 137929728
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
                parts_7 = 137937920
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

        UPDATE pangya.pangya_character_information
            SET 
                parts_8 = 137946112
        WHERE UID = _IDUSER AND typeid = _CHARTYPE;

    END IF;

    UPDATE pangya.pangya_character_information
        SET 
        default_shirts = _shirts
    WHERE UID = _IDUSER AND typeid = _CHARTYPE;

    UPDATE pangya.pangya_character_information
        SET 
        default_hair = _hairs
    WHERE UID = _IDUSER AND typeid = _CHARTYPE;
END;
$$;


ALTER FUNCTION pangya.funcinsertpartscharacter(_iduser integer, _chartype integer, _shirts integer, _hairs integer) OWNER TO postgres;

--
-- TOC entry 487 (class 1255 OID 23170)
-- Name: gerarankall(); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.gerarankall() RETURNS TABLE("_SUCCESS_" integer, _reg_date_ timestamp without time zone)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_CHECK bigint;
BEGIN
	-- Table variável
	CREATE TEMP TABLE _tabRank (
		index bigint NOT NULL DEFAULT (0),
		uid int NOT NULL DEFAULT (0),
		valor int NOT NULL DEFAULT (0)
	);
	
	-- Table RANK temp
	CREATE TEMP TABLE _tabRankTmp (
		index bigint NOT NULL GENERATED ALWAYS AS IDENTITY ( START 1 INCREMENT 1),
		position int NOT NULL DEFAULT (0),
		uid int NOT NULL DEFAULT (0),
		tipo_rank smallint NOT NULL DEFAULT (0),
		tipo_rank_seq smallint NOT NULL DEFAULT (0),
		valor int NOT NULL DEFAULT (0)
		
	);
	
	/*
    *    Info do rank normal - 0
    *    Dados Gerais 0
    *    Score 1 - (50 - (Score)) * 1 * Estrela
    */
    INSERT INTO _tabRank (index, UID, valor)
	SELECT
		ROW_NUMBER() OVER (ORDER BY X.Score DESC, X.UID) AS index,
		X.UID,
		X.Score
	FROM
	(
		SELECT
			UID, 
			(B.Score_1 + B.Score_2 + B.Score_3 + B.Score_4 + B.Score_5 + B.Score_7 + B.Score_8 + B.Score_9 + B.Score_10 + B.Score_11 + B.Score_12 + B.Score_13 + B.Score_14 + B.Score_15 + B.Score_16 + B.Score_17 + B.Score_18 + B.Score_19 + B.Score_20) AS Score
		FROM 
		(
			SELECT 
				A.uid, 
				MAX(CASE WHEN A.course = 0 THEN (50 - A.Score_1) * 10 ELSE 50 END) AS Score_1, 
				MAX(CASE WHEN A.course = 5 THEN (50 - A.Score_1) * 10 ELSE 50 END) AS Score_2, 
				MAX(CASE WHEN A.course = 11 THEN (50 - A.Score_1) * 10 ELSE 50 END) AS Score_3, 
				MAX(CASE WHEN A.course = 14 THEN (50 - A.Score_1) * 10 ELSE 50 END) AS Score_4, 
				MAX(CASE WHEN A.course = 15 THEN (50 - A.Score_1) * 10 ELSE 50 END) AS Score_5, 
				MAX(CASE WHEN A.course = 8 THEN (50 - A.Score_1) * 20 ELSE 50 END) AS Score_6, 
				MAX(CASE WHEN A.course = 10 THEN (50 - A.Score_1) * 20 ELSE 50 END) AS Score_7, 
				MAX(CASE WHEN A.course = 16 THEN (50 - A.Score_1) * 20 ELSE 50 END) AS Score_8, 
				MAX(CASE WHEN A.course = 19 THEN (50 - A.Score_1) * 20 ELSE 50 END) AS Score_9, 
				MAX(CASE WHEN A.course = 20 THEN (50 - A.Score_1) * 20 ELSE 50 END) AS Score_10, 
				MAX(CASE WHEN A.course = 1 THEN (50 - A.Score_1) * 30 ELSE 50 END) AS Score_11, 
				MAX(CASE WHEN A.course = 2 THEN (50 - A.Score_1) * 30 ELSE 50 END) AS Score_12, 
				MAX(CASE WHEN A.course = 6 THEN (50 - A.Score_1) * 30 ELSE 50 END) AS Score_13, 
				MAX(CASE WHEN A.course = 9 THEN (50 - A.Score_1) * 30 ELSE 50 END) AS Score_14, 
				MAX(CASE WHEN A.course = 21 THEN (50 - A.Score_1) * 30 ELSE 50 END) AS Score_15, 
				MAX(CASE WHEN A.course = 4 THEN (50 - A.Score_1) * 40 ELSE 50 END) AS Score_16, 
				MAX(CASE WHEN A.course = 7 THEN (50 - A.Score_1) * 40 ELSE 50 END) AS Score_17, 
				MAX(CASE WHEN A.course = 18 THEN (50 - A.Score_1) * 40 ELSE 50 END) AS Score_18, 
				MAX(CASE WHEN A.course = 3 THEN (50 - A.Score_1) * 50 ELSE 50 END) AS Score_19, 
				MAX(CASE WHEN A.course = 13 THEN (50 - A.Score_1) * 50 ELSE 50 END) AS Score_20
			FROM 
			(
				SELECT 
					f.course, 
					f.uid, 
					f.best_score AS Score_1
				FROM 
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
				WHERE 
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND best_score != 127 /*AND assist = 0*/ AND tipo IN(0, 51, 52) -- Normal, Natural e Grand Prix, soma todos
			) AS A
			GROUP BY A.uid
		) AS B
	) X
	ORDER BY X.Score DESC, X.UID;

	-- Insert Score
    INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 0, 1, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/* 
	* Trofeu 2 
	*/ 
	INSERT INTO _tabRank(index, uid, valor)
	SELECT
		ROW_NUMBER() OVER (ORDER BY C.SOMA DESC, C.UID) AS index,
		C.UID,
		C.SOMA
	FROM 
	(
		SELECT 
			a.uid,
			(a.AMA_6_G * 3 * 1) + (a.AMA_6_S * 2 * 1) + (a.AMA_6_B * 1 * 1) + (a.AMA_5_G * 3 * 1) + (a.AMA_5_S * 2 * 1) + (a.AMA_5_B * 1 * 1) + (a.AMA_4_G * 3 * 1) + (a.AMA_4_S * 2 * 1) + (a.AMA_4_B * 1 * 1) + (a.AMA_3_G * 3 * 2) + (a.AMA_3_S * 2 * 2) + (a.AMA_3_B * 1 * 2) + (a.AMA_2_G * 3 * 2) + (a.AMA_2_S * 2 * 2) + (a.AMA_2_B * 1 * 2) + (a.AMA_1_G * 3 * 2) + (a.AMA_1_S * 2 * 2) + (a.AMA_1_B * 1 * 2) + (a.PRO_1_G * 3 * 3) + (a.PRO_1_S * 2 * 3) + (a.PRO_1_B * 1 * 3) + (a.PRO_2_G * 3 * 3) + (a.PRO_2_S * 2 * 3) + (a.PRO_2_B * 1 * 3) + (a.PRO_3_G * 3 * 3) + (a.PRO_3_S * 2 * 3) + (a.PRO_3_B * 1 * 3) + (a.PRO_4_G * 3 * 4) + (a.PRO_4_S * 2 * 4) + (a.PRO_4_B * 1 * 4) + (a.PRO_5_G * 3 * 4) + (a.PRO_5_S * 2 * 4) + (a.PRO_5_B * 1 * 4) + (a.PRO_6_G * 3 * 4) + (a.PRO_6_S * 2 * 4) + (a.PRO_6_B * 1 * 4) + (a.PRO_7_G * 3 * 5) + (a.PRO_7_S * 2 * 5) + (a.PRO_7_B * 1 * 5) AS SOMA
		FROM 
			pangya.trofel_stat AS A
			INNER JOIN
			pangya.account AS B
			ON A.UID = B.UID
		WHERE (B.first_login + B.first_set) = 2
	) AS C
	ORDER BY C.SOMA DESC, C.uid;

	-- Insert Trofeu 2
    INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 0, 2, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;
     
	/*
	*    Total Pangs ganho em jogo 3 - Pangs / 1000
	*    Aqui estou usando o do pang total, mas tem que criar outra
	*/ 
	INSERT INTO _tabRank(index, UID, valor)
	SELECT 
		ROW_NUMBER() OVER (ORDER BY B.pangs DESC, b.UID) AS index,
		B.UID,
		B.pangs
	FROM 
	(
		SELECT 
			a.uid, 
			floor(a.total_pang_win_game / 1000) AS pangs
		FROM 
			pangya.user_info AS a
			INNER JOIN
			pangya.account AS b
			ON a.uid = b.UID
		WHERE
			(b.first_login + b.first_set) = 2
	)  B
	ORDER BY b.pangs DESC, b.uid;

	-- Insert Total Pangs 3
    INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 0, 3, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/* 
	* Holes 4 - (Holes - Holein) / 18
	*/ 
	INSERT INTO _tabRank(index, UID, valor)
	SELECT 
		ROW_NUMBER() OVER (ORDER BY B.holes DESC, b.UID) AS index,
		B.UID,
		B.holes
	FROM 
	(
		SELECT 
			a.uid, 
			floor((a.holes - a.holein) / 18) AS holes
		FROM 
			pangya.user_info a
			INNER JOIN
			pangya.account  b
			ON a.UID = b.UID
		WHERE
			(b.first_login + b.first_set) = 2
	) B
	ORDER BY b.holes DESC, b.uid;

	-- Insert Total Holes 4
    INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 0, 4, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;
     
	/* 
	* Achievement Pontos 5
	*/ 
	INSERT INTO _tabRank(index, UID, valor)
	SELECT 
		ROW_NUMBER() OVER (ORDER BY B.pontos DESC, b.UID) AS index,
		B.UID,
		B.pontos
	FROM
	(
		SELECT
			a.UID,
			COALESCE(X.pontos, 0) AS pontos
		FROM
			pangya.account a
			LEFT OUTER JOIN
		(
			SELECT
				b.UID,
				COALESCE(COUNT(b.uid), 0) * 10 AS pontos
			FROM
				pangya.pangya_achievement b
				LEFT OUTER JOIN
				pangya.pangya_quest c
				ON b.ID_ACHIEVEMENT = c.achievement_id
			WHERE 
				c.Date IS NOT NULL
			GROUP BY b.UID
		) X
		ON a.UID = X.UID
		WHERE (a.FIRST_LOGIN + a.FIRST_SET) = 2
	)  B
	ORDER BY b.pontos DESC, b.uid;

	-- Insert Total Achievements Points 5
    INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 0, 5, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	* A Soma de todos Dados Gerais 0
	*/ 
	INSERT  INTO _tabRank(index, UID, valor)
	SELECT 
		ROW_NUMBER() OVER (ORDER BY A.valor DESC) AS index,
		A.UID,
		A.valor
	FROM 
	(
		SELECT 
			uid, 
			SUM(valor) AS valor
		FROM 
			_tabRankTmp
		WHERE 
			tipo_rank = 0
		GROUP BY uid
	)  A
	ORDER BY A.valor DESC;

	-- Insert Total Overall 0
    INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 0, 0, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

   /*
   *    Infos hio alba level e total_distancia - 2
   *    Tem que colocar só os que fizeram o first login e character
   *    Albatross 0
   */ 
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
		ROW_NUMBER() OVER (ORDER BY a.albatross DESC, a.UID) AS index,
		a.uid,
		a.albatross
	FROM 
		pangya.user_info a
		INNER JOIN
		pangya.account b
		ON a.UID = b.UID
	WHERE 
		(b.first_login + b.first_set) = 2
	ORDER BY a.albatross DESC, a.uid;

	-- Insert Total Albatross 0
    INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 2, 0, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/* 
	* Hole In One 1
	*/ 
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
		ROW_NUMBER() OVER (ORDER BY a.hio DESC, a.UID) AS index,
		a.uid,
		a.hio
	FROM 
		pangya.user_info a
		INNER JOIN
		pangya.account b
		ON a.UID = b.UID
	WHERE 
		(b.first_login + b.first_set) = 2
	ORDER BY a.hio DESC, a.uid;

	-- Insert Total HIO 1
    INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 2, 1, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	* Level 3
	*/ 
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
		ROW_NUMBER() OVER (ORDER BY a.level DESC, a.xp DESC, a.UID) AS index,
		a.uid, 
		a.level
	FROM 
		pangya.user_info a
		INNER JOIN
		pangya.account b
		ON a.UID = b.UID
	WHERE 
		(b.first_login + b.first_set) = 2
	ORDER BY 
		a.level DESC, a.xp DESC, a.uid;

	-- Insert Total Level 3
    INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 2, 3, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	* Total distancia 4 - o PangYa faz a divisão por 10k sozinho
	*/ 
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
		ROW_NUMBER() OVER (ORDER BY a.total_distancia DESC, a.UID) AS index,
		a.uid,
		a.total_distancia
	FROM 
		pangya.user_info a
		INNER JOIN
		pangya.account b
		ON a.UID = b.UID
	WHERE 
		(b.first_login + b.first_set) = 2
	ORDER BY a.total_distancia DESC, a.uid;

	-- Insert Total Distância 4
    INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 2, 4, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;
   
	/*
	*       -------------- Normal --------------
	*       Nome            CODE            ESTRELA
	*       BL              0               *
	*       BW              1               ***
	*       SW              2               ***
	*       WH              3               *****
	*       WW              4               ****
	*       West            5               *
	*       BM              6               ***
	*       SC              7               ****
	*       IC              8               **
	*       WhW             9               ***
	*       SS              10              **
	*       PW              11              *
	*       DI              13              *****
	*       IS              14              *
	*       LS              15              *
	*       EV              16              **
	*       IF              18              ****
	*       WC              19              **
	*       AM              20              **
	*       MR              21              ***
	*       -----------------*------------------
	**/
	/*
	*   RECORD 1
	*   BL 0
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 0 AND f.tipo = 0
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (BL) 0
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 1, 0, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 1
	*   BW 1
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 1 AND f.tipo = 0
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (BW) 1
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 1, 1, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 1
	*   SW 2
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 2 AND f.tipo = 0
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (SW) 2
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 1, 2, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 1
	*   WH 3
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 3 AND f.tipo = 0
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (WH) 3
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 1, 3, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 1
	*   WW 4
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 4 AND f.tipo = 0
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (WW) 4
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 1, 4, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 1
	*   West 5
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 5 AND f.tipo = 0
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (West) 5
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 1, 5, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 1
	*   BM 6
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 6 AND f.tipo = 0
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (BM) 6
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 1, 6, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 1
	*   SC 7
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 7 AND f.tipo = 0
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (SC) 7
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 1, 7, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 1
	*   IC 8
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 8 AND f.tipo = 0
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (IC) 8
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 1, 8, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 1
	*   WhW 9
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 9 AND f.tipo = 0
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (WhW) 9
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 1, 9, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 1
	*   SS 10
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 10 AND f.tipo = 0
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (SS) 10
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 1, 10, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 1
	*   PW 11
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 11 AND f.tipo = 0
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (PW) 11
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 1, 11, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 1
	*   DI 13
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 13 AND f.tipo = 0
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (DI) 13
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 1, 12, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 1
	*   IS 14
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 14 AND f.tipo = 0
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (IS) 14
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 1, 13, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 1
	*   LS 15
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 15 AND f.tipo = 0
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (LS) 15
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 1, 14, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 1
	*   EV 16
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 16 AND f.tipo = 0
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (EV) 16
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 1, 15, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 1
	*   IF 18
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 18 AND f.tipo = 0
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (IF) 18
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 1, 16, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 1
	*   WC 19
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 19 AND f.tipo = 0
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (WC) 19
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 1, 17, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 1
	*   AM 20
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 20 AND f.tipo = 0
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (AM) 20
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 1, 18, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 1
	*   MR 21
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 21 AND f.tipo = 0
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (MR) 21
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 1, 19, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*       -------------- Natural --------------
	*       Nome            CODE            ESTRELA
	*       BL              0               *
	*       BW              1               ***
	*       SW              2               ***
	*       WH              3               *****
	*       WW              4               ****
	*       West            5               *
	*       BM              6               ***
	*       SC              7               ****
	*       IC              8               **
	*       WhW             9               ***
	*       SS              10              **
	*       PW              11              *
	*       DI              13              *****
	*       IS              14              *
	*       LS              15              *
	*       EV              16              **
	*       IF              18              ****
	*       WC              19              **
	*       AM              20              **
	*       MR              21              ***
	*       -----------------*------------------
	**/
	/*
	*   RECORD 3
	*   BL 0
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 0 AND f.tipo = 51
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (BL) 0
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 3, 0, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 3
	*   BW 1
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 1 AND f.tipo = 51
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (BW) 1
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 3, 1, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 3
	*   SW 2
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 2 AND f.tipo = 51
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (SW) 2
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 3, 2, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 3
	*   WH 3
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 3 AND f.tipo = 51
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (WH) 3
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 3, 3, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 3
	*   WW 4
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 4 AND f.tipo = 51
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (WW) 4
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 3, 4, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 3
	*   West 5
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 5 AND f.tipo = 51
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (West) 5
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 3, 5, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 3
	*   BM 6
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 6 AND f.tipo = 51
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (BM) 6
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 3, 6, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 3
	*   SC 7
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 7 AND f.tipo = 51
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (SC) 7
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 3, 7, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 3
	*   IC 8
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 8 AND f.tipo = 51
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (IC) 8
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 3, 8, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 3
	*   WhW 9
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 9 AND f.tipo = 51
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (WhW) 9
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 3, 9, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 3
	*   SS 10
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 10 AND f.tipo = 51
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (SS) 10
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 3, 10, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 3
	*   PW 11
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 11 AND f.tipo = 51
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (PW) 11
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 3, 11, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 3
	*   DI 13
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 13 AND f.tipo = 51
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (DI) 13
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 3, 12, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 3
	*   IS 14
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 14 AND f.tipo = 51
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (IS) 14
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 3, 13, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 3
	*   LS 15
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 15 AND f.tipo = 51
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (LS) 15
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 3, 14, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 3
	*   EV 16
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 16 AND f.tipo = 51
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (EV) 16
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 3, 15, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 3
	*   IF 18
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 18 AND f.tipo = 51
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (IF) 18
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 3, 16, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 3
	*   WC 19
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 19 AND f.tipo = 51
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (WC) 19
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 3, 17, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 3
	*   AM 20
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 20 AND f.tipo = 51
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (AM) 20
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 3, 18, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 3
	*   MR 21
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 21 AND f.tipo = 51
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (MR) 21
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 3, 19, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*       -------------- Grand Prix --------------
	*       Nome            CODE            ESTRELA
	*       BL              0               *
	*       BW              1               ***
	*       SW              2               ***
	*       WH              3               *****
	*       WW              4               ****
	*       West            5               *
	*       BM              6               ***
	*       SC              7               ****
	*       IC              8               **
	*       WhW             9               ***
	*       SS              10              **
	*       PW              11              *
	*       DI              13              *****
	*       IS              14              *
	*       LS              15              *
	*       EV              16              **
	*       IF              18              ****
	*       WC              19              **
	*       AM              20              **
	*       MR              21              ***
	*       -----------------*------------------
	**/
	/*
	*   RECORD 4
	*   BL 0
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 0 AND f.tipo = 52
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (BL) 0
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 4, 0, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 4
	*   BW 1
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 1 AND f.tipo = 52
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (BW) 1
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 4, 1, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 4
	*   SW 2
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 2 AND f.tipo = 52
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (SW) 2
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 4, 2, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 4
	*   WH 3
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 3 AND f.tipo = 52
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (WH) 3
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 4, 3, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 4
	*   WW 4
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 4 AND f.tipo = 52
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (WW) 4
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 4, 4, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 4
	*   West 5
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 5 AND f.tipo = 52
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (West) 5
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 4, 5, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 4
	*   BM 6
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 6 AND f.tipo = 52
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (BM) 6
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 4, 6, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 4
	*   SC 7
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 7 AND f.tipo = 52
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (SC) 7
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 4, 7, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 4
	*   IC 8
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 8 AND f.tipo = 52
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (IC) 8
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 4, 8, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 4
	*   WhW 9
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 9 AND f.tipo = 52
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (WhW) 9
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 4, 9, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 4
	*   SS 10
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 10 AND f.tipo = 52
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (SS) 10
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 4, 10, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 4
	*   PW 11
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 11 AND f.tipo = 52
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (PW) 11
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 4, 11, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 4
	*   DI 13
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 13 AND f.tipo = 52
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (DI) 13
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 4, 12, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 4
	*   IS 14
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 14 AND f.tipo = 52
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (IS) 14
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 4, 13, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 4
	*   LS 15
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 15 AND f.tipo = 52
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (LS) 15
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 4, 14, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 4
	*   EV 16
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 16 AND f.tipo = 52
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (EV) 16
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 4, 15, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 4
	*   IF 18
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 18 AND f.tipo = 52
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (IF) 18
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 4, 16, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 4
	*   WC 19
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 19 AND f.tipo = 52
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (WC) 19
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 4, 17, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 4
	*   AM 20
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 20 AND f.tipo = 52
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (AM) 20
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 4, 18, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	/*
	*   RECORD 4
	*   MR 21
	*/
	INSERT INTO _tabRank(index, UID, valor)
	SELECT
			ROW_NUMBER() OVER (ORDER BY Z.best_score, Z.best_pang DESC, Z.UID) AS index,
			Z.UID,
			Z.best_score
	FROM
	(
			SELECT
					f.uid,
					MIN(f.best_score) as best_score,
					MAX(f.best_pang) as best_pang
			FROM
					pangya.pangya_record f
					INNER JOIN
					pangya.account g
					ON f.UID = g.UID
			WHERE
					(g.FIRST_LOGIN + g.FIRST_SET) = 2
					AND f.best_score != 127 /*AND f.assist = 0*/
					AND f.course = 21 AND f.tipo = 52
			GROUP BY f.UID
	) Z
	ORDER BY
			Z.best_score,
			Z.best_pang DESC,
			Z.uid;

	-- Insert Best Score Course (MR) 21
	INSERT INTO _tabRankTmp(position, UID, tipo_rank, tipo_rank_seq, valor)
	SELECT index, UID, 4, 19, valor FROM _tabRank;

	-- Clear _tabRank
	DELETE FROM _tabRank;

	_CHECK = (SELECT COUNT(index) FROM _tabRankTmp);

	-- Verifica se enseriu os dados para a table rank tmp tudo ok
	IF _CHECK IS NOT NULL AND _CHECK > 0 AND _CHECK >= (SELECT COUNT(index) FROM pangya.pangya_rank_atual) THEN
	BEGIN
		
		-- TUDO OK Exclui os dados antigos que estavam no rank_antes
		-- e passa o atual para o antes e passa o temp para o atual
		
		-- Rank Antes
		DELETE FROM pangya.pangya_rank_antes;
		
		ALTER TABLE pangya.pangya_rank_antes ALTER COLUMN index RESTART SET START 1;

		INSERT INTO pangya.pangya_rank_antes(position, UID, tipo_rank, tipo_rank_seq, valor)
		SELECT position, UID, tipo_rank, tipo_rank_seq, valor FROM pangya.pangya_rank_atual;

		-- Rank Atual
		DELETE FROM pangya.pangya_rank_atual;

		ALTER TABLE pangya.pangya_rank_atual ALTER COLUMN index RESTART SET START 1;

		INSERT INTO pangya.pangya_rank_atual(position, UID, tipo_rank, tipo_rank_seq, valor)
		SELECT position, UID, tipo_rank, tipo_rank_seq, valor FROM _tabRankTmp;

		-- Free tmp
		DELETE FROM _tabRankTmp;

		-- Update rank Character Equiped
		--------- Aqui atualizar o character que o player estava no momento -----------

		-- Delete os antigos
		DELETE FROM pangya.pangya_rank_atual_character;
      
		-- Insere os novos
		INSERT INTO pangya.pangya_rank_atual_character(	uid, 
													item_id, 
													typeid, 
													itemid_parts_1, 
													itemid_parts_2, 
													itemid_parts_3, 
													itemid_parts_4, 
													itemid_parts_5, 
													itemid_parts_6, 
													itemid_parts_7, 
													itemid_parts_8, 
													itemid_parts_9, 
													itemid_parts_10, 
													itemid_parts_11, 
													itemid_parts_12, 
													itemid_parts_13, 
													itemid_parts_14, 
													itemid_parts_15, 
													itemid_parts_16, 
													itemid_parts_17, 
													itemid_parts_18, 
													itemid_parts_19, 
													itemid_parts_20, 
													itemid_parts_21, 
													itemid_parts_22, 
													itemid_parts_23, 
													itemid_parts_24, 
													parts_1, 
													parts_2, 
													parts_3, 
													parts_4, 
													parts_5, 
													parts_6, 
													parts_7, 
													parts_8, 
													parts_9, 
													parts_10, 
													parts_11, 
													parts_12, 
													parts_13, 
													parts_14, 
													parts_15, 
													parts_16, 
													parts_17, 
													parts_18, 
													parts_19, 
													parts_20, 
													parts_21, 
													parts_22, 
													parts_23, 
													parts_24, 
													default_hair, 
													default_shirts, 
													gift_flag, 
													PCL0, 
													PCL1, 
													PCL2, 
													PCL3, 
													PCL4, 
													purchase, 
													AUXPARTS_1, 
													AUXPARTS_2, 
													AUXPARTS_3, 
													AUXPARTS_4, 
													AUXPARTS_5, 
													CutIn_1,
													CutIn_2,
													CutIn_3,
													CutIn_4, 
													mastery, 
													CARD_CHARACTER_1, 
													CARD_CHARACTER_2, 
													CARD_CHARACTER_3, 
													CARD_CHARACTER_4, 
													CARD_CADDIE_1, 
													CARD_CADDIE_2, 
													CARD_CADDIE_3, 
													CARD_CADDIE_4, 
													CARD_NPC_1, 
													CARD_NPC_2, 
													CARD_NPC_3, 
													CARD_NPC_4
		)
		SELECT 
			X.UID, 
			X.item_id, 
			X.typeid, 
			Y.ITEMID_PARTS_1, 
			Y.ITEMID_PARTS_2, 
			Y.ITEMID_PARTS_3, 
			Y.ITEMID_PARTS_4, 
			Y.ITEMID_PARTS_5, 
			Y.ITEMID_PARTS_6, 
			Y.ITEMID_PARTS_7, 
			Y.ITEMID_PARTS_8, 
			Y.ITEMID_PARTS_9, 
			Y.ITEMID_PARTS_10, 
			Y.ITEMID_PARTS_11, 
			Y.ITEMID_PARTS_12, 
			Y.ITEMID_PARTS_13, 
			Y.ITEMID_PARTS_14, 
			Y.ITEMID_PARTS_15, 
			Y.ITEMID_PARTS_16, 
			Y.ITEMID_PARTS_17, 
			Y.ITEMID_PARTS_18, 
			Y.ITEMID_PARTS_19, 
			Y.ITEMID_PARTS_20, 
			Y.ITEMID_PARTS_21, 
			Y.ITEMID_PARTS_22, 
			Y.ITEMID_PARTS_23, 
			Y.ITEMID_PARTS_24, 
			X.parts_1, 
			X.parts_2, 
			X.parts_3, 
			X.parts_4, 
			X.parts_5, 
			X.parts_6, 
			X.parts_7, 
			X.parts_8, 
			X.parts_9, 
			X.parts_10, 
			X.parts_11, 
			X.parts_12, 
			X.parts_13, 
			X.parts_14, 
			X.parts_15, 
			X.parts_16, 
			X.parts_17, 
			X.parts_18, 
			X.parts_19, 
			X.parts_20, 
			X.parts_21, 
			X.parts_22, 
			X.parts_23, 
			X.parts_24, 
			X.default_hair, 
			X.default_shirts, 
			X.gift_flag, 
			X.PCL0, 
			X.PCL1, 
			X.PCL2, 
			X.PCL3, 
			X.PCL4, 
			X.Purchase, 
			X.auxparts_1, 
			X.auxparts_2, 
			X.auxparts_3, 
			X.auxparts_4, 
			X.auxparts_5, 
			X.CutIn_1, 
			X.CutIn_2, 
			X.CutIn_3, 
			X.CutIn_4, 
			X.Mastery, 
			Z.CARD_CHARACTER_1, 
			Z.CARD_CHARACTER_2, 
			Z.CARD_CHARACTER_3, 
			Z.CARD_CHARACTER_4, 
			Z.CARD_CADDIE_1, 
			Z.CARD_CADDIE_2, 
			Z.CARD_CADDIE_3, 
			Z.CARD_CADDIE_4, 
			Z.CARD_NPC_1, 
			Z.CARD_NPC_2, 
			Z.CARD_NPC_3, 
			Z.CARD_NPC_4
		FROM 
		(
			SELECT 
				pangya_character_information.UID, 
				pangya_character_information.item_id, 
				pangya_character_information.typeid, 
				pangya_character_information.parts_1, 
				pangya_character_information.parts_2, 
				pangya_character_information.parts_3, 
				pangya_character_information.parts_4, 
				pangya_character_information.parts_5, 
				pangya_character_information.parts_6, 
				pangya_character_information.parts_7, 
				pangya_character_information.parts_8, 
				pangya_character_information.parts_9, 
				pangya_character_information.parts_10, 
				pangya_character_information.parts_11, 
				pangya_character_information.parts_12, 
				pangya_character_information.parts_13, 
				pangya_character_information.parts_14, 
				pangya_character_information.parts_15, 
				pangya_character_information.parts_16, 
				pangya_character_information.parts_17, 
				pangya_character_information.parts_18, 
				pangya_character_information.parts_19, 
				pangya_character_information.parts_20, 
				pangya_character_information.parts_21, 
				pangya_character_information.parts_22, 
				pangya_character_information.parts_23, 
				pangya_character_information.parts_24, 
				pangya_character_information.auxparts_1, 
				pangya_character_information.auxparts_2, 
				pangya_character_information.auxparts_3, 
				pangya_character_information.auxparts_4, 
				pangya_character_information.auxparts_5, 
				pangya_character_information.default_hair, 
				pangya_character_information.default_shirts, 
				pangya_character_information.gift_flag, 
				pangya_character_information.PCL0, 
				pangya_character_information.PCL1, 
				pangya_character_information.PCL2, 
				pangya_character_information.PCL3, 
				pangya_character_information.PCL4, 
				pangya_character_information.Purchase, 
				pangya_character_information.CutIn_1, 
				pangya_character_information.CutIn_2,
				pangya_character_information.CutIn_3,
				pangya_character_information.CutIn_4,
				pangya_character_information.Mastery
			FROM 
				pangya.pangya_character_information
		) AS X 
		INNER JOIN 
		(
			SELECT
				A.item_id, 
				MAX(CASE WHEN B.EQUIP_NUM = 1 THEN B.ITEMID ELSE 0 END) AS ITEMID_PARTS_1, 
				MAX(CASE WHEN B.EQUIP_NUM = 2 THEN B.ITEMID ELSE 0 END) AS ITEMID_PARTS_2, 
				MAX(CASE WHEN B.EQUIP_NUM = 3 THEN B.ITEMID ELSE 0 END) AS ITEMID_PARTS_3, 
				MAX(CASE WHEN B.EQUIP_NUM = 4 THEN B.ITEMID ELSE 0 END) AS ITEMID_PARTS_4, 
				MAX(CASE WHEN B.EQUIP_NUM = 5 THEN B.ITEMID ELSE 0 END) AS ITEMID_PARTS_5, 
				MAX(CASE WHEN B.EQUIP_NUM = 6 THEN B.ITEMID ELSE 0 END) AS ITEMID_PARTS_6, 
				MAX(CASE WHEN B.EQUIP_NUM = 7 THEN B.ITEMID ELSE 0 END) AS ITEMID_PARTS_7, 
				MAX(CASE WHEN B.EQUIP_NUM = 8 THEN B.ITEMID ELSE 0 END) AS ITEMID_PARTS_8, 
				MAX(CASE WHEN B.EQUIP_NUM = 9 THEN B.ITEMID ELSE 0 END) AS ITEMID_PARTS_9, 
				MAX(CASE WHEN B.EQUIP_NUM = 10 THEN B.ITEMID ELSE 0 END) AS ITEMID_PARTS_10, 
				MAX(CASE WHEN B.EQUIP_NUM = 11 THEN B.ITEMID ELSE 0 END) AS ITEMID_PARTS_11, 
				MAX(CASE WHEN B.EQUIP_NUM = 12 THEN B.ITEMID ELSE 0 END) AS ITEMID_PARTS_12, 
				MAX(CASE WHEN B.EQUIP_NUM = 13 THEN B.ITEMID ELSE 0 END) AS ITEMID_PARTS_13, 
				MAX(CASE WHEN B.EQUIP_NUM = 14 THEN B.ITEMID ELSE 0 END) AS ITEMID_PARTS_14, 
				MAX(CASE WHEN B.EQUIP_NUM = 15 THEN B.ITEMID ELSE 0 END) AS ITEMID_PARTS_15, 
				MAX(CASE WHEN B.EQUIP_NUM = 16 THEN B.ITEMID ELSE 0 END) AS ITEMID_PARTS_16, 
				MAX(CASE WHEN B.EQUIP_NUM = 17 THEN B.ITEMID ELSE 0 END) AS ITEMID_PARTS_17, 
				MAX(CASE WHEN B.EQUIP_NUM = 18 THEN B.ITEMID ELSE 0 END) AS ITEMID_PARTS_18, 
				MAX(CASE WHEN B.EQUIP_NUM = 19 THEN B.ITEMID ELSE 0 END) AS ITEMID_PARTS_19, 
				MAX(CASE WHEN B.EQUIP_NUM = 20 THEN B.ITEMID ELSE 0 END) AS ITEMID_PARTS_20, 
				MAX(CASE WHEN B.EQUIP_NUM = 21 THEN B.ITEMID ELSE 0 END) AS ITEMID_PARTS_21, 
				MAX(CASE WHEN B.EQUIP_NUM = 22 THEN B.ITEMID ELSE 0 END) AS ITEMID_PARTS_22, 
				MAX(CASE WHEN B.EQUIP_NUM = 23 THEN B.ITEMID ELSE 0 END) AS ITEMID_PARTS_23, 
				MAX(CASE WHEN B.EQUIP_NUM = 24 THEN B.ITEMID ELSE 0 END) AS ITEMID_PARTS_24
			FROM 
			(
				SELECT 
					pangya_character_information.item_id
				FROM 
					pangya.pangya_character_information
			) AS A 
			LEFT OUTER JOIN 
			(
				SELECT 
					td_char_equip_s4.CHAR_ITEMID,
					td_char_equip_s4.ITEMID,
					td_char_equip_s4.EQUIP_NUM
				FROM 
					pangya.td_char_equip_s4
				WHERE 
					td_char_equip_s4.USE_YN = 'Y'
			) AS B 
			ON A.item_id = B.CHAR_ITEMID
			GROUP BY A.item_id
		) AS Y
		ON X.item_id = Y.item_id
		INNER JOIN 
		(
			SELECT 
				E.item_id, 
				MAX(CASE WHEN F.Tipo = 0 AND F.Slot = 1 THEN F.card_typeid ELSE 0 END) AS CARD_CHARACTER_1, 
				MAX(CASE WHEN F.Tipo = 0 AND F.Slot = 2 THEN F.card_typeid ELSE 0 END) AS CARD_CHARACTER_2, 
				MAX(CASE WHEN F.Tipo = 0 AND F.Slot = 3 THEN F.card_typeid ELSE 0 END) AS CARD_CHARACTER_3, 
				MAX(CASE WHEN F.Tipo = 0 AND F.Slot = 4 THEN F.card_typeid ELSE 0 END) AS CARD_CHARACTER_4, 
				MAX(CASE WHEN F.Tipo = 1 AND F.Slot = 5 THEN F.card_typeid ELSE 0 END) AS CARD_CADDIE_1, 
				MAX(CASE WHEN F.Tipo = 1 AND F.Slot = 6 THEN F.card_typeid ELSE 0 END) AS CARD_CADDIE_2, 
				MAX(CASE WHEN F.Tipo = 1 AND F.Slot = 7 THEN F.card_typeid ELSE 0 END) AS CARD_CADDIE_3, 
				MAX(CASE WHEN F.Tipo = 1 AND F.Slot = 8 THEN F.card_typeid ELSE 0 END) AS CARD_CADDIE_4, 
				MAX(CASE WHEN F.Tipo = 5 AND F.Slot = 9 THEN F.card_typeid ELSE 0 END) AS CARD_NPC_1, 
				MAX(CASE WHEN F.Tipo = 5 AND F.Slot = 10 THEN F.card_typeid ELSE 0 END) AS CARD_NPC_2, 
				MAX(CASE WHEN F.Tipo = 5 AND F.Slot = 11 THEN F.card_typeid ELSE 0 END) AS CARD_NPC_3, 
				MAX(CASE WHEN F.Tipo = 5 AND F.Slot = 12 THEN F.card_typeid ELSE 0 END) AS CARD_NPC_4
			FROM 
			(
				SELECT 
					pangya_character_information.item_id
				FROM 
					pangya.pangya_character_information
			) AS E 
			LEFT OUTER JOIN 
			(
				SELECT 
					pangya_card_equip.Tipo, 
					pangya_card_equip.Slot, 
					pangya_card_equip.card_typeid, 
					pangya_card_equip.parts_id
				FROM 
					pangya.pangya_card_equip
				WHERE 
					pangya_card_equip.USE_YN = 1
			) AS F 
			ON F.parts_id = E.item_id
			GROUP BY E.item_id
		) AS Z
		ON Y.item_id = Z.item_id
		INNER JOIN 
		(
			SELECT 
				ff.character_id
			FROM 
				pangya.pangya_user_equip AS ff
				INNER JOIN
				pangya.account AS bb
				ON ff.UID = bb.UID
			WHERE 
				(bb.FIRST_LOGIN + bb.FIRST_SET) = 2
		) AS W 
		ON Z.item_id = W.character_id
		ORDER BY X.UID;

		-- Update Time Refresh Rank Generation
		UPDATE 
			pangya.pangya_rank_config
		SET 
			reg_date = now();

		-- Fim
		RETURN QUERY SELECT 
			1 AS SUCCESS,
			reg_date
		FROM
			pangya.pangya_rank_config;

	END;
	ELSE
		-- Error
		RETURN QUERY SELECT 0 AS SUCCESS, null::timestamp;
	END IF;
	
	DROP TABLE _tabRank;
	DROP TABLE _tabRankTmp;
END;
$$;


ALTER FUNCTION pangya.gerarankall() OWNER TO postgres;

--
-- TOC entry 488 (class 1255 OID 23172)
-- Name: gerarankall_ant(); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.gerarankall_ant() RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	DELETE FROM pangya.pangya_rank_antes;

	INSERT INTO pangya.pangya_rank_antes(
		position, 
		UID, 
		tipo_rank, 
		tipo_rank_seq, 
		valor)
	SELECT 
		position, 
		UID, 
		tipo_rank, 
		tipo_rank_seq, 
		valor
	FROM pangya.pangya_rank_atual;

	DELETE 
	FROM pangya.pangya_rank_atual;

	DELETE 
	FROM pangya.pangya_rank_atual_character;
	
	UPDATE pangya.pangya_rank_config
		SET 
			reg_date = now();
END;
$$;


ALTER FUNCTION pangya.gerarankall_ant() OWNER TO postgres;

--
-- TOC entry 489 (class 1255 OID 23173)
-- Name: getexpbylevel(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.getexpbylevel(_iduser integer) RETURNS integer
    LANGUAGE plpgsql
    AS $$
DECLARE 
	_EXP INT default 0;
	_LVL INT;
BEGIN
	SELECT level INTO _LVL FROM user_info WHERE UID = _IDUSER;

	IF _LVL = 0 THEN
		_EXP = 30;
	ELSIF _LVL = 1 THEN
		_EXP = 40;
	ELSIF _LVL = 2 THEN
		_EXP = 50;
	ELSIF _LVL = 3 THEN 
		_EXP = 60;
	ELSIF _LVL = 4 THEN 
		_EXP = 70;
	ELSIF _LVL = 5 THEN 
		_EXP = 140;
	ELSIF _LVL = 6 THEN 
		_EXP = 105;
	ELSIF _LVL = 7 THEN 
		_EXP = 125;
	ELSIF _LVL = 8 THEN 
		_EXP = 145;
	ELSIF _LVL = 9 THEN 
		_EXP = 165;
	ELSIF _LVL = 10 THEN 
		_EXP = 330;
	ELSIF _LVL = 11 THEN 
		_EXP = 248;
	ELSIF _LVL = 12 THEN 
		_EXP = 278;
	ELSIF _LVL = 13 THEN 
		_EXP = 308;
	ELSIF _LVL = 14 THEN 
		_EXP = 338;
	ELSIF _LVL = 15 THEN 
		_EXP = 675;
	ELSIF _LVL = 16 THEN 
		_EXP = 506;
	ELSIF _LVL = 17 THEN 
		_EXP = 546;
	ELSIF _LVL = 18 THEN 
		_EXP = 586;
	ELSIF _LVL = 19 THEN 
		_EXP = 626;
	ELSIF _LVL = 20 THEN 
		_EXP = 1253;
	ELSIF _LVL = 21 THEN 
		_EXP = 1002;
	ELSIF _LVL = 22 THEN 
		_EXP = 1052;
	ELSIF _LVL = 23 THEN 
		_EXP = 1102;
	ELSIF _LVL = 24 THEN 
		_EXP = 1152;
	ELSIF _LVL = 25 THEN 
		_EXP = 2304;
	ELSIF _LVL = 26 THEN 
		_EXP = 1843;
	ELSIF _LVL = 27 THEN 
		_EXP = 1903;
	ELSIF _LVL = 28 THEN 
		_EXP = 1963;
	ELSIF _LVL = 29 THEN 
		_EXP = 2023;
	ELSIF _LVL = 30 THEN 
		_EXP = 4046;
	ELSIF _LVL = 31 THEN 
		_EXP = 3237;
	ELSIF _LVL = 32 THEN 
		_EXP = 3307;
	ELSIF _LVL = 33 THEN 
		_EXP = 3377;
	ELSIF _LVL = 34 THEN 
		_EXP = 3447;
	ELSIF _LVL = 35 THEN 
		_EXP = 6894;
	ELSIF _LVL = 36 THEN 
		_EXP = 5515;
	ELSIF _LVL = 37 THEN 
		_EXP = 5595;
	ELSIF _LVL = 38 THEN 
		_EXP = 5675;
	ELSIF _LVL = 39 THEN 
		_EXP = 5755;
	ELSIF _LVL = 40 THEN 
		_EXP = 11511;
	ELSIF _LVL = 41 THEN 
		_EXP = 8058;
	ELSIF _LVL = 42 THEN 
		_EXP = 8148;
	ELSIF _LVL = 43 THEN 
		_EXP = 8238;
	ELSIF _LVL = 44 THEN 
		_EXP = 8328;
	ELSIF _LVL = 45 THEN 
		_EXP = 16655;
	ELSIF _LVL = 46 THEN 
		_EXP = 8328;
	ELSIF _LVL = 47 THEN 
		_EXP = 8428;
	ELSIF _LVL = 48 THEN 
		_EXP = 8528;
	ELSIF _LVL = 49 THEN 
		_EXP = 8628;
	ELSIF _LVL = 50 THEN 
		_EXP = 17255;
	ELSIF _LVL = 51 THEN 
		_EXP = 9490;
	ELSIF _LVL = 52 THEN 
		_EXP = 9690;
	ELSIF _LVL = 53 THEN 
		_EXP = 9890;
	ELSIF _LVL = 54 THEN 
		_EXP = 10090;
	ELSIF _LVL = 55 THEN 
		_EXP = 20181;
	ELSIF _LVL = 56 THEN 
		_EXP = 20181;
	ELSIF _LVL = 57 THEN 
		_EXP = 20481;
	ELSIF _LVL = 58 THEN 
		_EXP = 20781;
	ELSIF _LVL = 59 THEN 
		_EXP = 21081;
	ELSIF _LVL = 60 THEN 
		_EXP = 42161;
	ELSIF _LVL = 61 THEN 
		_EXP = 37945;
	ELSIF _LVL = 62 THEN 
		_EXP = 68301;
	ELSIF _LVL = 63 THEN 
		_EXP = 122942;
	ELSIF _LVL = 64 THEN 
		_EXP = 221296;
	ELSIF _LVL = 65 THEN 
		_EXP = 442592;
	ELSIF _LVL = 66 THEN 
		_EXP = 663887;
	ELSIF _LVL = 67 THEN 
		_EXP = 995831;
	ELSIF _LVL = 68 THEN 
		_EXP = 1493747;
	ELSIF _LVL = 69 THEN 
		_EXP = 2240620;
	ELSIF _LVL >= 70 THEN 
		_EXP = 999999999;
    END IF;
	
	RETURN _EXP;
END;
$$;


ALTER FUNCTION pangya.getexpbylevel(_iduser integer) OWNER TO postgres;

--
-- TOC entry 491 (class 1255 OID 23174)
-- Name: getinfo_user(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.getinfo_user(_iduser integer) RETURNS TABLE("_UID_" bigint, "_Tacadas_" bigint, "_Putt_" bigint, "_Tempo_" bigint, "_Tempo tacadas_" bigint, "_Max_distancia_" real, "_Acerto_pangya_" bigint, "_Bunker_" integer, "_O.B_" bigint, "_Total_distancia_" bigint, "_Holes_" bigint, "_Holein_" integer, "_HIO_" bigint, "_Timeout_" smallint, "_Fairway_" bigint, "_Albatross_" bigint, "_MaConduta_" integer, "_Acerto_Putt_" bigint, "_Long-putt_" real, "_Chip-in_" real, "_Xp_" bigint, _level_ smallint, "_Pang_" numeric, "_Media_score_" integer, "_BestScore0_" smallint, "_BestScore1_" smallint, "_BestScore2_" smallint, "_BestScore3_" smallint, "_BestScore4_" smallint, "_MaxPang0_" bigint, "_maxPang1_" bigint, "_maxPang2_" bigint, "_maxPang3_" bigint, "_maxPang4_" bigint, "_SumPang_" bigint, "_EventFlag_" smallint, "_Jogado_" bigint, "_Quitado_" bigint, "_SkinPang_" bigint, "_SkinWin_" integer, "_SkinLose_" integer, "_SkinRunHole_" integer, "_SkinStrikePoint_" integer, "_SkinAllinCount_" integer, "_Todos_combos_" bigint, "_Combos_" bigint, "_TeamWin_" integer, "_TeamGames_" integer, "_Teamhole_" bigint, "_LadderPoint_" integer, "_LadderWin_" integer, "_LadderLose_" integer, "_LadderDraw_" integer, "_LadderHole_" integer, "_EventValue_" smallint, "_NaoSei_" integer, "_MaxJogoNaoSei_" integer, "_JogosNaoSei_" integer, "_GameCountSeason_" integer, "_Cookie_" numeric, _lucky_medal_ integer, _fast_medal_ integer, _best_drive_medal_ integer, _best_chipin_medal_ integer, _best_puttin_medal_ integer, _best_recovery_medal_ integer, _16bit_naosei_ smallint, _total_pang_win_game_ bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT 
        UID, 
        Tacadas, 
        Putt, 
        Tempo, 
        "Tempo tacadas", 
        Max_distancia, 
        Acerto_pangya, 
        Bunker, 
        "O.B", 
        Total_distancia, 
        Holes, 
        Holein, 
        HIO, 
        Timeout, 
        Fairway, 
        Albatross, 
        MaConduta, 
        Acerto_Putt, 
        "Long-putt", 
        "Chip-in", 
        Xp, 
        level, 
        Pang, 
        Media_score, 
        BestScore0, 
        BestScore1, 
        BestScore2, 
        BestScore3, 
        BestScore4, 
        MaxPang0, 
        maxPang1, 
        maxPang2, 
        maxPang3, 
        maxPang4, 
        SumPang, 
        EventFlag, 
        Jogado, 
        Quitado, 
        SkinPang, 
        SkinWin, 
        SkinLose, 
        SkinRunHole, 
        SkinStrikePoint, 
        SkinAllinCount, 
        Todos_combos, 
        Combos, 
        TeamWin, 
        TeamGames, 
        Teamhole, 
        LadderPoint, 
        LadderWin, 
        LadderLose, 
        LadderDraw, 
        LadderHole, 
        EventValue, 
        NaoSei, 
        MaxJogoNaoSei, 
        JogosNaoSei, 
        GameCountSeason, 
        Cookie, 
        lucky_medal, 
        fast_medal, 
        best_drive_medal, 
        best_chipin_medal, 
        best_puttin_medal, 
        best_recovery_medal, 
        "16bit_naosei",
        total_pang_win_game
    FROM pangya.user_info
    WHERE user_info.UID = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.getinfo_user(_iduser integer) OWNER TO postgres;

--
-- TOC entry 492 (class 1255 OID 23175)
-- Name: getlevelmemorial(); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.getlevelmemorial() RETURNS TABLE(_id_achievement_ bigint, _id_achievementm_ bigint, "_UID_" integer, _typeid_ integer, _dup_ bigint)
    LANGUAGE plpgsql
    AS $$
	
BEGIN

	CREATE TEMP TABLE _lastIDs (id BIGINT);
	CREATE TEMP TABLE _ff (id_achievement BIGINT, id_achievementm BIGINT, UID INT, typeid INT, dup BIGINT);
	
	INSERT INTO _ff SELECT
		MAX(ID_ACHIEVEMENT) AS ID_ACHIEVEMENT,
		MIN(ID_ACHIEVEMENT) AS ID_ACHIEVEMENTM,
		UID
		,TypeID
		, COUNT(*) AS DUP
	FROM pangya.pangya_achievement
	WHERE TypeID < x'78000000'::int
	GROUP BY TypeID, UID
	HAVING COUNT(*) > 1
	ORDER BY UID LIMIT 10000;

	-- SHOW
	RETURN QUERY SELECT * FROM _ff;

	-- DELETE
	DELETE FROM pangya.pangya_quest WHERE achievement_id in(SELECT id_achievement FROM _ff);
	DELETE FROM pangya.pangya_achievement WHERE ID_ACHIEVEMENT in(SELECT ID_ACHIEVEMENT FROM _ff);
	
	-- DROP TEMPORARY TABLE
	DROP TABLE _lastIDs;
	DROP TABLE _ff;
END;
$$;


ALTER FUNCTION pangya.getlevelmemorial() OWNER TO postgres;

--
-- TOC entry 493 (class 1255 OID 23176)
-- Name: gettutorial(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.gettutorial(_iduser integer) RETURNS TABLE("_Rookie_" integer, "_Beginner_" integer, "_Advancer_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT Rookie, Beginner, Advancer
      FROM pangya.tutorial
      WHERE UID = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.gettutorial(_iduser integer) OWNER TO postgres;

--
-- TOC entry 494 (class 1255 OID 23177)
-- Name: procaddball(integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procaddball(_iduser integer, _giftflag integer, _purchase_in integer, _iditem integer, _itemtypeid integer, _typeflag integer, _tempo integer, _c_0 integer, _c_1 integer, _c_2 integer, _c_3 integer, _c_4 integer) RETURNS TABLE("_ITEM_ID" bigint)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_ITEM_ID BIGINT = -1;
BEGIN
	INSERT INTO pangya.pangya_item_warehouse(UID, typeid, valid, regdate, Gift_flag, flag, C0, Purchase, ItemType)
							VALUES(_IDUSER, _ITEMTYPEID, 1, now(), _GIFTFLAG, _TYPEFLAG, _C_0, 0, 2)
							RETURNING item_id INTO _ITEM_ID;

	RETURN QUERY SELECT _ITEM_ID AS ITEM_ID;
END;
$$;


ALTER FUNCTION pangya.procaddball(_iduser integer, _giftflag integer, _purchase_in integer, _iditem integer, _itemtypeid integer, _typeflag integer, _tempo integer, _c_0 integer, _c_1 integer, _c_2 integer, _c_3 integer, _c_4 integer) OWNER TO postgres;

--
-- TOC entry 495 (class 1255 OID 23178)
-- Name: procaddcaddie(integer, integer, integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procaddcaddie(_iduser integer, _iditem integer, __typeid integer, _gift_flag integer, _purchase integer, _rent_flag integer, _tempo integer) RETURNS TABLE("_ITEM_ID" bigint, "_ENDDATE_" timestamp without time zone)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_ITEM_ID BIGINT = -1;
BEGIN
	IF _IDITEM <= 0 THEN
		INSERT INTO pangya.pangya_caddie_information(UID, typeid, gift_flag, Purchase, RentFlag, Period, RegDate, EndDate)
			VALUES(_IDUSER, __TYPEID, _GIFT_FLAG, _PURCHASE, _RENT_FLAG, _TEMPO, now(), (now() + ('1 day'::interval * _TEMPO)))
			RETURNING item_id INTO _ITEM_ID;

		RETURN QUERY SELECT item_id, EndDate FROM pangya.pangya_caddie_information WHERE item_id = _ITEM_ID;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procaddcaddie(_iduser integer, _iditem integer, __typeid integer, _gift_flag integer, _purchase integer, _rent_flag integer, _tempo integer) OWNER TO postgres;

--
-- TOC entry 496 (class 1255 OID 23179)
-- Name: procaddcharacter(integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procaddcharacter(_iduser integer, _iditem integer, __typeid integer, _default_hair integer, _default_shirts integer, _purchage integer, _gift_flag integer, _part1 integer, _part2 integer, _part3 integer, _part4 integer, _part5 integer, _part6 integer, _part7 integer, _part8 integer, _part9 integer, _part10 integer, _part11 integer, _part12 integer, _part13 integer, _part14 integer, _part15 integer, _part16 integer, _part17 integer, _part18 integer, _part19 integer, _part20 integer, _part21 integer, _part22 integer, _part23 integer, _part24 integer) RETURNS TABLE(_item_id integer)
    LANGUAGE plpgsql
    AS $$
BEGIN

	IF _IDITEM <= 0 THEN
		_IDITEM = COALESCE((SELECT item_id FROM pangya.pangya_character_information WHERE UID = _IDUSER AND typeid = __TYPEID), -1);
	END IF;

	IF _IDITEM <= 0 THEN
	DECLARE _COUNT INT DEFAULT 0;
	BEGIN
		INSERT INTO pangya.pangya_character_information(UID, TYPEID, default_hair, default_shirts, Purchase, gift_flag, 
			parts_1, parts_2, parts_3, parts_4, parts_5, parts_6, parts_7, parts_8, parts_9, parts_10, parts_11, parts_12,
			parts_13, parts_14, parts_15, parts_16, parts_17, parts_18, parts_19, parts_20, parts_21, parts_22, parts_23, parts_24) 
		VALUES(_IDUSER, __TYPEID, _DEFAULT_HAIR, _DEFAULT_SHIRTS, _PURCHAGE, _GIFT_FLAG, _PART1, _PART2, _PART3, _PART4, _PART5, _PART6, _PART7, _PART8,
				_PART9, _PART10, _PART11, _PART12, _PART13, _PART14, _PART15, _PART16, _PART17, _PART18, _PART19, _PART20, _PART21, _PART22, _PART23, _PART24)
		RETURNING item_id INTO _IDITEM; -- Character ID
		
		WHILE _COUNT < 24 LOOP
			INSERT INTO pangya.td_char_equip_s4(UID, CHAR_ITEMID, IN_DATE, EQUIP_NUM, USE_YN)
				VALUES(_IDUSER, _IDITEM, now(), _COUNT + 1, 'Y');
			_COUNT = _COUNT + 1;
		END LOOP;
	END;
	END IF;

	RETURN QUERY SELECT _IDITEM AS ITEM_ID;
END;
$$;


ALTER FUNCTION pangya.procaddcharacter(_iduser integer, _iditem integer, __typeid integer, _default_hair integer, _default_shirts integer, _purchage integer, _gift_flag integer, _part1 integer, _part2 integer, _part3 integer, _part4 integer, _part5 integer, _part6 integer, _part7 integer, _part8 integer, _part9 integer, _part10 integer, _part11 integer, _part12 integer, _part13 integer, _part14 integer, _part15 integer, _part16 integer, _part17 integer, _part18 integer, _part19 integer, _part20 integer, _part21 integer, _part22 integer, _part23 integer, _part24 integer) OWNER TO postgres;

--
-- TOC entry 497 (class 1255 OID 23180)
-- Name: procaddcounteritem(bigint, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procaddcounteritem(_iduser bigint, _itemtypeid integer, _active integer, _count_num_item integer) RETURNS TABLE("_COUNTER_ITEM_ID_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_COUNT_ID INT = -1;
BEGIN
	
	INSERT INTO pangya.pangya_counter_item(UID, TypeID, active, Count_Num_Item) VALUES(_IDUSER, _ITEMTYPEID, _ACTIVE, _COUNT_NUM_ITEM)
	RETURNING count_id INTO _COUNT_ID;
	
	RETURN QUERY SELECT _COUNT_ID AS COUNTER_ITEM_ID;
END;
$$;


ALTER FUNCTION pangya.procaddcounteritem(_iduser bigint, _itemtypeid integer, _active integer, _count_num_item integer) OWNER TO postgres;

--
-- TOC entry 473 (class 1255 OID 23181)
-- Name: procaddfriend(integer, integer, text, integer, integer, integer, integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procaddfriend(_iduser integer, _friend_uid integer, _friend_apelido text, _friend_unk1 integer, _friend_unk2 integer, _friend_unk3 integer, _friend_unk4 integer, _friend_unk5 integer, _friend_unk6 integer, _friend_flag1 integer, _friend_state integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDUSER > 0 AND _FRIEND_UID > 0 AND (SELECT UID FROM pangya.pangya_friend_list WHERE UID = _IDUSER AND uid_friend = _FRIEND_UID) IS NULL THEN
		INSERT INTO pangya.pangya_friend_list(UID, uid_friend, apelido, unknown1, unknown2, unknown3, unknown4, unknown5, unknown6, flag1, state_flag, flag5)
			VALUES(_IDUSER, _FRIEND_UID, _FRIEND_APELIDO, _FRIEND_UNK1, _FRIEND_UNK2, _FRIEND_UNK3, _FRIEND_UNK4, _FRIEND_UNK5, _FRIEND_UNK6, _FRIEND_FLAG1, _FRIEND_STATE, 0);
	END IF;
END;
$$;


ALTER FUNCTION pangya.procaddfriend(_iduser integer, _friend_uid integer, _friend_apelido text, _friend_unk1 integer, _friend_unk2 integer, _friend_unk3 integer, _friend_unk4 integer, _friend_unk5 integer, _friend_unk6 integer, _friend_flag1 integer, _friend_state integer) OWNER TO postgres;

--
-- TOC entry 490 (class 1255 OID 23182)
-- Name: procaddfurniture(integer, integer, double precision, double precision, double precision, double precision); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procaddfurniture(_iduser integer, _itemtypeid integer, _x double precision, _y double precision, _z double precision, _r double precision) RETURNS TABLE("_ITEM_ID" bigint)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_ITEM_ID BIGINT = -1;
BEGIN
	IF _IDUSER > 0 THEN
		INSERT INTO pangya.td_room_data(UID, TYPEID, POS_X, POS_Y, POS_Z, POS_R)
			VALUES(_IDUSER, _ITEMTYPEID, _X, _Y, _Z, _R)
			RETURNING myroom_id INTO _ITEM_ID;
	END IF;

	RETURN QUERY SELECT _ITEM_ID AS ITEM_ID;
END;
$$;


ALTER FUNCTION pangya.procaddfurniture(_iduser integer, _itemtypeid integer, _x double precision, _y double precision, _z double precision, _r double precision) OWNER TO postgres;

--
-- TOC entry 499 (class 1255 OID 23183)
-- Name: procadditem(integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procadditem(_iduser integer, _giftflag integer, _purchase_in integer, _iditem integer, _itemtypeid integer, _typeflag integer, _tempo integer, _c_0 integer, _c_1 integer, _c_2 integer, _c_3 integer, _c_4 integer) RETURNS TABLE("_ITEM_ID" bigint)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_IDITEM BIGINT = -1;
BEGIN
	IF _TEMPO > 0 THEN
		INSERT INTO pangya_item_warehouse(UID, typeid, valid, regdate, EndDate, Gift_flag, flag, C0, C1, C2, C3, C4, Purchase, ItemType)
				VALUES(_IDUSER, _ITEMTYPEID, 1, now(), (now() + ('1 day'::interval * _TEMPO)), _GIFTFLAG, _TYPEFLAG, _C_0, _C_1, _C_2, _C_3, _C_4, 0, 2)
				RETURNING item_id INTO _IDITEM;
    ELSE
		INSERT INTO pangya_item_warehouse(UID, typeid, valid, regdate, Gift_flag, flag, C0, C1, C2, C3, C4, Purchase, ItemType)
				VALUES(_IDUSER, _ITEMTYPEID, 1, now(), _GIFTFLAG, _TYPEFLAG, _C_0, _C_1, _C_2, _C_3, _C_4, 0, 2)
				RETURNING item_id INTO _IDITEM;
	END IF;

	RETURN QUERY SELECT _IDITEM AS ITEM_ID;
END;
$$;


ALTER FUNCTION pangya.procadditem(_iduser integer, _giftflag integer, _purchase_in integer, _iditem integer, _itemtypeid integer, _typeflag integer, _tempo integer, _c_0 integer, _c_1 integer, _c_2 integer, _c_3 integer, _c_4 integer) OWNER TO postgres;

--
-- TOC entry 500 (class 1255 OID 23184)
-- Name: procadditemdolfinilocker(integer, bigint); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procadditemdolfinilocker(_iduser integer, _itemid bigint) RETURNS TABLE("_INDEX_" bigint)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_idx bigint;
BEGIN
	UPDATE pangya.pangya_item_warehouse SET valid = 0
	WHERE UID = _IDUSER AND item_id = _ITEMID;

	INSERT INTO pangya.pangya_dolfini_locker_item(UID, item_id) VALUES (_IDUSER, _ITEMID)
	RETURNING idx INTO _idx;
	
	RETURN QUERY SELECT _idx AS index;
END;
$$;


ALTER FUNCTION pangya.procadditemdolfinilocker(_iduser integer, _itemid bigint) OWNER TO postgres;

--
-- TOC entry 501 (class 1255 OID 23185)
-- Name: procaddmsgoff(integer, integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procaddmsgoff(_iduser integer, _to_uid integer, _msg text) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDUSER > 0 AND _TO_UID > 0 THEN
		INSERT INTO pangya.pangya_msg_user(UID, uid_from, valid, msg, reg_date)
			VALUES(_TO_UID, _IDUSER, 1, _MSG, now());
	END IF;
END;
$$;


ALTER FUNCTION pangya.procaddmsgoff(_iduser integer, _to_uid integer, _msg text) OWNER TO postgres;

--
-- TOC entry 502 (class 1255 OID 23186)
-- Name: procaddpart(integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procaddpart(_iduser integer, _giftflag integer, _purchase_in integer, _iditem integer, _itemtypeid integer, _typeflag integer, _typeflag_iff integer, _tempo integer, _c_0 integer, _c_1 integer, _c_2 integer, _c_3 integer, _c_4 integer) RETURNS TABLE("_ITEM_ID_" bigint, "_UCC_IDX" character varying, "_SD_SEQ_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_IDITEM BIGINT = -1;
	_UCC_IDX varchar(8) = null::varchar;
	_SD_SEQ int = 0;
BEGIN
	IF _TYPEFLAG_IFF = 8 OR _TYPEFLAG_IFF = 9 THEN -- UCC
		IF _IDITEM <= 0 THEN
			SELECT * INTO _IDITEM, _UCC_IDX, _SD_SEQ FROM pangya.USP_ADD_UCC(_IDUSER, _ITEMTYPEID, _GIFTFLAG, _C_0, _C_1, _C_2, _C_3, _C_4, _PURCHASE_IN, _TYPEFLAG_IFF, _TEMPO);
		END IF;
	ELSE	-- Normal Item
							
		IF _TEMPO > 0 THEN
			INSERT INTO pangya.pangya_item_warehouse(UID, typeid, valid, regdate, Gift_flag, flag, Purchase, ItemType, EndDate, c0, c1, c2, c3, c4)
						VALUES(_IDUSER, _ITEMTYPEID, 1, now(), _GIFTFLAG, _TYPEFLAG, 0, 2, (now() + ('1 day'::interval * _TEMPO)), _C_0, _C_1, _C_2, _C_3, _C_4)
						RETURNING item_id INTO _IDITEM;
		ELSE
			INSERT INTO pangya.pangya_item_warehouse(UID, typeid, valid, regdate, enddate, Gift_flag, flag, Purchase, ItemType, c0, c1, c2, c3, c4)
						VALUES(_IDUSER, _ITEMTYPEID, 1, now(), now(), _GIFTFLAG, _TYPEFLAG, 0, 2, _C_0, _C_1, _C_2, _C_3, _C_4)
						RETURNING item_id INTO _IDITEM;
		END IF;
	END IF;

	RETURN QUERY SELECT _IDITEM AS ITEM_ID, _UCC_IDX AS UCC_IDX, _SD_SEQ AS SD_SEQ;
END;
$$;


ALTER FUNCTION pangya.procaddpart(_iduser integer, _giftflag integer, _purchase_in integer, _iditem integer, _itemtypeid integer, _typeflag integer, _typeflag_iff integer, _tempo integer, _c_0 integer, _c_1 integer, _c_2 integer, _c_3 integer, _c_4 integer) OWNER TO postgres;

--
-- TOC entry 503 (class 1255 OID 23187)
-- Name: procaddtrofelgrandprix(bigint, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procaddtrofelgrandprix(_iduser bigint, _itemtypeid integer, _qntd integer) RETURNS TABLE("_ITEM_ID_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_ITEM_ID INT = -1;
BEGIN
	
	INSERT INTO pangya.pangya_trofel_grandprix(UID, TYPEID, qntd) VALUES(_IDUSER, _ITEMTYPEID, _QNTD)
	RETURNING item_id INTO _ITEM_ID;
	
	RETURN QUERY SELECT _ITEM_ID AS ITEM_ID;
END;
$$;


ALTER FUNCTION pangya.procaddtrofelgrandprix(_iduser bigint, _itemtypeid integer, _qntd integer) OWNER TO postgres;

--
-- TOC entry 504 (class 1255 OID 23188)
-- Name: procaddtrofelspecial(bigint, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procaddtrofelspecial(_iduser bigint, _itemtypeid integer, _qntd integer) RETURNS TABLE("_ITEM_ID_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_ITEM_ID INT = -1;
BEGIN
	
	INSERT INTO pangya.pangya_trofel_especial(UID, TYPEID, qntd) VALUES(_IDUSER, _ITEMTYPEID, _QNTD)
	RETURNING item_id INTO _ITEM_ID;
	
	RETURN QUERY SELECT _ITEM_ID AS ITEM_ID;
END;
$$;


ALTER FUNCTION pangya.procaddtrofelspecial(_iduser bigint, _itemtypeid integer, _qntd integer) OWNER TO postgres;

--
-- TOC entry 505 (class 1255 OID 23189)
-- Name: procadduserfriend(integer, integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procadduserfriend(_iduser integer, _friend_uid integer, _nickname text) RETURNS TABLE("_OPT_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_OPT int = 0;
BEGIN
	IF 
	(
		SELECT account.UID
		FROM pangya.account
		WHERE account.UID = _friend_uid AND account.NICK = _nickname
	) IS NOT NULL THEN

		IF 
		(
			SELECT count(pangya_friend_list.uid_friend)
			FROM pangya.pangya_friend_list
			WHERE pangya_friend_list.uid = _friend_uid
		) < 50 THEN

			IF 
			(
				SELECT pangya_friend_list.uid
				FROM pangya.pangya_friend_list
				WHERE pangya_friend_list.uid = _IDUSER AND pangya_friend_list.uid_friend = _friend_uid
			) IS NULL THEN
			BEGIN

				INSERT INTO pangya.pangya_friend_list(pangya.pangya_friend_list.uid, pangya.pangya_friend_list.uid_friend, pangya.pangya_friend_list.state_flag)
				VALUES (_IDUSER, _friend_uid, 8);

				INSERT INTO pangya.pangya_friend_list(pangya.pangya_friend_list.uid, pangya.pangya_friend_list.uid_friend, pangya.pangya_friend_list.state_flag)
				VALUES (_friend_uid, _IDUSER, 0);

			END;
			ELSE 
				_OPT = 2;
			END IF;
		ELSE 
			_OPT = 3;
		END IF;
	ELSE 
		_OPT = 1;
	END IF;

	RETURN QUERY SELECT _OPT AS retorno;
END;
$$;


ALTER FUNCTION pangya.procadduserfriend(_iduser integer, _friend_uid integer, _nickname text) OWNER TO postgres;

--
-- TOC entry 506 (class 1255 OID 23190)
-- Name: procblockplayerluciaattendance(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procblockplayerluciaattendance(_uid integer, _block_type integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _UID > 0 AND _BLOCK_TYPE > 0 THEN
	DECLARE
		DECLARE _END_DATE timestamp = (CASE _BLOCK_TYPE
												WHEN 1 THEN
													(now() + '1 day'::interval)
												WHEN 2 THEN
									   				(now() + '1 week'::interval)
												WHEN 3 THEN
									   				(now() + '1 month'::interval)
												ELSE NULL
											END);
	BEGIN

		UPDATE pangya.pangya_lucia_attendance 
			SET block_type = _BLOCK_TYPE, 
				block_end_date = _END_DATE,
				try_hacking_count = 0
		WHERE UID = _UID;
	END;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procblockplayerluciaattendance(_uid integer, _block_type integer) OWNER TO postgres;

--
-- TOC entry 507 (class 1255 OID 23191)
-- Name: proccanceluplevelclubset(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.proccanceluplevelclubset(_iduser integer, _taqueira_id integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	UPDATE pangya.pangya_item_warehouse
         SET 
            Recovery_Pts = pangya_item_warehouse.Recovery_Pts + 1
      WHERE pangya_item_warehouse.UID = _IDUSER AND pangya_item_warehouse.item_id = _TAQUEIRA_ID;
END;
$$;


ALTER FUNCTION pangya.proccanceluplevelclubset(_iduser integer, _taqueira_id integer) OWNER TO postgres;

--
-- TOC entry 508 (class 1255 OID 23192)
-- Name: procchangedolfinilockerpass(integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procchangedolfinilockerpass(_uid integer, _pass text) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _UID > 0 AND _PASS != '' THEN
		UPDATE pangya.pangya_dolfini_locker SET senha = _PASS WHERE UID = _UID;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procchangedolfinilockerpass(_uid integer, _pass text) OWNER TO postgres;

--
-- TOC entry 509 (class 1255 OID 23193)
-- Name: procchangenickname(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procchangenickname(_uid integer) RETURNS TABLE("_RET_" integer, "_CP_" bigint)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_RET INT = -1;
	_CP BIGINT = 0;
BEGIN
	IF _UID > 0 THEN
	BEGIN

		IF (SELECT FIRST_LOGIN FROM pangya.account WHERE UID = _UID) = 1 THEN
		BEGIN

			IF (SELECT (CASE WHEN change_nick IS NULL THEN 1 ELSE DATEDIFF('MONTH', change_nick, now()::timestamp) END) as diff FROM pangya.account WHERE UID = _UID) > 0 THEN
			BEGIN
		
				IF (SELECT cookie FROM pangya.user_info WHERE uid = _UID) >= 60 THEN
				BEGIN

					-- Update cookie, tira os 60 CP que gastar para trocar o nickname
					UPDATE pangya.user_info SET Cookie = Cookie - 60 WHERE UID = _UID;

					-- Atualiza CP que vai ser retornado
					_CP = (SELECT cookie FROM pangya.user_info WHERE uid = _UID);

					-- Insere o nickname antigo no log de change nickname
					INSERT INTO pangya.pangya_change_nickname_log(UID, nickname) SELECT UID, NICK FROM pangya.account WHERE UID = _UID LIMIT 1;

					-- Atualiza a flag de first login e a data que o player pediu para trocar de nickname
					UPDATE pangya.account 
						SET FIRST_LOGIN = 0,			-- Ele pode trocar o nick quando ele logar no jogo
							change_nick = now()		-- Atualiza a data que ele atualizou o nickname
					WHERE UID = _UID;

					_RET = 1;	-- Fez o pedido de troca de nickname com sucesso

				END;
				ELSE
					_RET = -2;	-- Não tem cookies suficiente
				END IF;

			END;
			ELSE
				_RET = -3;	-- Ainda não deu um mês desde que ele trocou o nickname
			END IF;

		END;
		ELSE
			_RET = -4;	-- Já pediu para trocar o nickname
		END IF;

	END;
	END IF;

	RETURN QUERY SELECT _RET as RET, _CP as cookie;
END;
$$;


ALTER FUNCTION pangya.procchangenickname(_uid integer) OWNER TO postgres;

--
-- TOC entry 510 (class 1255 OID 23194)
-- Name: procchangepassword(integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procchangepassword(_uid integer, _pwd text) RETURNS TABLE("_RET_" integer, "_EMAIL_" character varying)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_RET INT = -1;
	_EMAIL VARCHAR(100) = '';
BEGIN
	IF _UID > 0 AND _PWD != '' THEN
	BEGIN

		-- Pega email do player
		_EMAIL = (SELECT email FROM pangya.contas_beta WHERE UID = _UID);

		-- Verifica se tem log de troca de senha
		IF (SELECT UID FROM pangya.pangya_change_pwd_log WHERE UID = _UID) IS NOT NULL THEN
		DECLARE 
			_COUNT INT = 0;
			_CHANGE_DATE timestamp;
		BEGIN

			-- Pega o contador de vez que o player trocou o login e a primeira data que ele trocou
			SELECT count, change_date INTO _COUNT, _CHANGE_DATE
			FROM pangya.pangya_change_pwd_log
			WHERE UID = _UID;
			
			IF DATEDIFF('MINUTE', _CHANGE_DATE, now()::timestamp) >= 1440 THEN -- 24 horas em minutos
			BEGIN
			
				-- Faz mais de 24 horas que ele trocou de senha	
				UPDATE pangya.pangya_change_pwd_log 
					SET last_change = now(),
						change_date = now(),
						count = 1
				WHERE UID = _UID;

				-- Troca a senha
				UPDATE pangya.account SET PASSWORD = _PWD WHERE UID = _UID;

				_RET = 1;

			END;
			ELSIF _COUNT = 1 THEN
			BEGIN
				
				-- Menos de 24 horas que ele trocou de senha uma vez
				UPDATE pangya.pangya_change_pwd_log
					SET last_change = now(),
						count = _COUNT + 1
				WHERE UID = _UID;

				-- Troca a senha
				UPDATE pangya.account SET PASSWORD = _PWD WHERE UID = _UID;

				_RET = 1;

			END;
			ELSE
				_RET = -2;
			END IF;

		END;
		ELSE
			
			-- Ele não tem log de troca de senha, cria um novo para ele
			INSERT INTO pangya.pangya_change_pwd_log(UID) VALUES(_UID);

			-- Troca a senha
			UPDATE pangya.account SET PASSWORD = _PWD WHERE UID = _UID;

			_RET = 1;

		END IF;
		
	END;
	END IF;

	RETURN QUERY SELECT _RET AS RET, _EMAIL AS EMAIL;
END;
$$;


ALTER FUNCTION pangya.procchangepassword(_uid integer, _pwd text) OWNER TO postgres;

--
-- TOC entry 511 (class 1255 OID 23195)
-- Name: proccheckachievement(bigint); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.proccheckachievement(_uid bigint) RETURNS TABLE("_COUNT_" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT COUNT(UID) AS COUNT FROM pangya.pangya_achievement WHERE UID = _UID;
END;
$$;


ALTER FUNCTION pangya.proccheckachievement(_uid bigint) OWNER TO postgres;

--
-- TOC entry 512 (class 1255 OID 23196)
-- Name: proccheckdailyquest(); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.proccheckdailyquest() RETURNS TABLE("_DATE_" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF 
	(
		SELECT count(*)
		FROM pangya.pangya_daily_quest
	) > 0 THEN
		RETURN QUERY SELECT COALESCE(datediff('day', pangya_daily_quest.Reg_Date, now()), 1) AS DATE
		FROM pangya.pangya_daily_quest;
	ELSE 
		RETURN QUERY SELECT 1 AS DATE;
	END IF;
END;
$$;


ALTER FUNCTION pangya.proccheckdailyquest() OWNER TO postgres;

--
-- TOC entry 513 (class 1255 OID 23197)
-- Name: proccheckguildname(text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.proccheckguildname(_name text) RETURNS TABLE("_GUILD_UID_" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT GUILD_UID FROM pangya.pangya_guild WHERE GUILD_NAME = _NAME;
END;
$$;


ALTER FUNCTION pangya.proccheckguildname(_name text) OWNER TO postgres;

--
-- TOC entry 514 (class 1255 OID 23198)
-- Name: proccheckplayerdailyquest(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.proccheckplayerdailyquest(_iduser integer) RETURNS TABLE("_RET_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_RET int = 0;
BEGIN
	IF 
	(
		SELECT pangya_daily_quest_player.today_quest
		FROM pangya.pangya_daily_quest_player
		WHERE pangya_daily_quest_player.uid = _IDUSER
	) IS NULL THEN
		_RET = 1;
	ELSE 
	BEGIN
		IF 
		(
			SELECT date_part('day', pangya_daily_quest_player.today_quest)
			FROM pangya.pangya_daily_quest_player
			WHERE pangya_daily_quest_player.uid = _IDUSER
		) <> 
		(
			SELECT date_part('day', pangya_daily_quest.Reg_Date)
			FROM pangya.pangya_daily_quest
		) THEN
			_RET = 1;
		END IF;
	END;
	END IF;

	RETURN QUERY SELECT _RET;
END;
$$;


ALTER FUNCTION pangya.proccheckplayerdailyquest(_iduser integer) OWNER TO postgres;

--
-- TOC entry 515 (class 1255 OID 23199)
-- Name: proccheckrankrefreshtime(); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.proccheckrankrefreshtime() RETURNS TABLE("_RET_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT
         CASE 
            WHEN (reg_date + ('1 hour'::interval * refresh_time_H)) < now() THEN 1
            ELSE 0
         END AS booleann
      FROM pangya.pangya_rank_config LIMIT 1;
END;
$$;


ALTER FUNCTION pangya.proccheckrankrefreshtime() OWNER TO postgres;

--
-- TOC entry 516 (class 1255 OID 23200)
-- Name: proccheckrescuepasswordandidkey(bigint, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.proccheckrescuepasswordandidkey(_index bigint, _key text) RETURNS TABLE("_RET_" integer, "_UID_" integer, "_TIPO_" smallint)
    LANGUAGE plpgsql
    AS $$
DECLARE 
	_RET INT = -1;
	_UID INT = 0;
	_TIPO SMALLINT = 0;
BEGIN
	IF _INDEX > 0 AND _KEY != '' THEN
	BEGIN

		_UID = (SELECT UID FROM pangya.pangya_rescue_pwd_log WHERE index = _INDEX AND key_uniq = _KEY::uuid AND state = 0 AND (send_date + '1 hour'::interval) > now()::timestamp);

		IF _UID IS NOT NULL AND _UID > 0 THEN
		BEGIN

			-- Pega o tipo de recuperação se é de senha ou de id
			SELECT tipo INTO _TIPO FROM pangya.pangya_rescue_pwd_log WHERE index = _INDEX AND UID = _UID;

			-- Atualiza o state da chave para utilizada
			IF _TIPO = 2 THEN -- ID Faz agora por que ele só precisa ver o id, e não trocar ele como a senha
				UPDATE pangya.pangya_rescue_pwd_log SET state = 1 WHERE index = _INDEX AND UID = _UID;
			END IF;

			-- Verificou a chave com sucesso
			_RET = 1;

		END;
		ELSE
			_RET = -2;	-- Chave invalida, não existe a chave, já foi utilizada ou  ela expirou
		END IF;

	END;
	END IF;

	RETURN QUERY SELECT _RET AS RET, _UID as UID, _TIPO AS TIPO;
END;
$$;


ALTER FUNCTION pangya.proccheckrescuepasswordandidkey(_index bigint, _key text) OWNER TO postgres;

--
-- TOC entry 498 (class 1255 OID 23201)
-- Name: procchecksecuritykey(integer, integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procchecksecuritykey(_iduser integer, _iditem integer, _key text) RETURNS TABLE("_UID_" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT UID
	FROM pangya.tu_ucc
	WHERE 
		tu_ucc.UID = _IDUSER AND 
		tu_ucc.ITEM_ID = _IDITEM AND 
		tu_ucc.SKEY = _KEY;
END;
$$;


ALTER FUNCTION pangya.procchecksecuritykey(_iduser integer, _iditem integer, _key text) OWNER TO postgres;

--
-- TOC entry 460 (class 1255 OID 23202)
-- Name: proccheckuccdraws(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.proccheckuccdraws(_iduser integer) RETURNS TABLE("_ITEM_ID_" numeric)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT ITEM_ID
		FROM pangya.tu_ucc
		WHERE tu_ucc.UID = _IDUSER AND DRAW_DT IS NOT NULL;
END;
$$;


ALTER FUNCTION pangya.proccheckuccdraws(_iduser integer) OWNER TO postgres;

--
-- TOC entry 517 (class 1255 OID 23203)
-- Name: proccolocamsgnogifttable(integer, integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.proccolocamsgnogifttable(_from_uid integer, _to_uid integer, _msg text) RETURNS TABLE("_MSG_ID_" bigint)
    LANGUAGE plpgsql
    AS $$
DECLARE 
	_NICKNAME VARCHAR(100);
	_MSG_ID bigint = 0;
BEGIN
	SELECT NICK INTO _NICKNAME FROM pangya.account WHERE UID = _From_UID;

	IF _NICKNAME IS NULL OR _NICKNAME = '' THEN
		_NICKNAME = '@SUPERSS';
	END IF;

	INSERT INTO pangya.pangya_gift_table(
		UID, 
		fromid, 
		message, 
		giftdate, 
		Lida_YN, 
		valid, 
		Flag, 
		Contador_Vista)
	VALUES (
		_To_UID, 
		_NICKNAME, 
		_MSG, 
		now(), 
		0, 
		1, 
		0, 
		0)
	RETURNING msg_id INTO _MSG_ID;

	RETURN QUERY SELECT _MSG_ID AS MSG_ID;
END;
$$;


ALTER FUNCTION pangya.proccolocamsgnogifttable(_from_uid integer, _to_uid integer, _msg text) OWNER TO postgres;

--
-- TOC entry 518 (class 1255 OID 23204)
-- Name: procdeletaquestanterior(); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procdeletaquestanterior() RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	DELETE 
	FROM pangya.achievement_quest
	WHERE achievement_quest.ID_ACHIEVEMENT IN 
		(
			SELECT achievement_tipo.ID_ACHIEVEMENT
			FROM pangya.achievement_tipo
			WHERE achievement_tipo.Option = 1
		);

	DELETE 
	FROM pangya.achievement_tipo
	WHERE achievement_tipo.Option = 1;
END;
$$;


ALTER FUNCTION pangya.procdeletaquestanterior() OWNER TO postgres;

--
-- TOC entry 519 (class 1255 OID 23205)
-- Name: procdeleteblockipfromtable(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procdeleteblockipfromtable(_index integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _INDEX > 0 THEN
		
		DELETE FROM pangya.pangya_ip_table WHERE index = _INDEX;

	END IF;
END;
$$;


ALTER FUNCTION pangya.procdeleteblockipfromtable(_index integer) OWNER TO postgres;

--
-- TOC entry 520 (class 1255 OID 23206)
-- Name: procdeleteblockmacfromtable(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procdeleteblockmacfromtable(_index integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _INDEX > 0 THEN
		DELETE FROM pangya.pangya_mac_table WHERE index = _INDEX;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procdeleteblockmacfromtable(_index integer) OWNER TO postgres;

--
-- TOC entry 521 (class 1255 OID 23207)
-- Name: procdeletebotgmeventreward(bigint); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procdeletebotgmeventreward(_id bigint) RETURNS TABLE("_ID_" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _ID > 0 THEN
	BEGIN

		DELETE FROM pangya.pangya_bot_gm_event_reward WHERE index = _ID;

		RETURN QUERY SELECT _ID AS ID;

	END;
	ELSE
		RETURN QUERY SELECT -1 AS ID;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procdeletebotgmeventreward(_id bigint) OWNER TO postgres;

--
-- TOC entry 522 (class 1255 OID 23208)
-- Name: procdeletebotgmeventtime(bigint); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procdeletebotgmeventtime(_id bigint) RETURNS TABLE("_ID_" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _ID > 0 THEN
	BEGIN

		DELETE FROM pangya.pangya_bot_gm_event_time WHERE index = _ID;

		RETURN QUERY SELECT _ID AS ID;

	END;
	ELSE
		RETURN QUERY SELECT -1 AS ID;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procdeletebotgmeventtime(_id bigint) OWNER TO postgres;

--
-- TOC entry 523 (class 1255 OID 23209)
-- Name: procdeletedonationbygm(bigint); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procdeletedonationbygm(_id bigint) RETURNS TABLE("_ID_" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _ID != -1 AND (SELECT index FROM pangya.pangya_donation_new WHERE index = _ID) IS NOT NULL THEN
	BEGIN

		DELETE FROM pangya.pangya_donation_new WHERE index = _ID;

		RETURN QUERY SELECT _ID AS ID;

	END;
	ELSE
		RETURN QUERY SELECT -1 AS ID;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procdeletedonationbygm(_id bigint) OWNER TO postgres;

--
-- TOC entry 524 (class 1255 OID 23210)
-- Name: procdeletegoldentimeevent(bigint); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procdeletegoldentimeevent(_id bigint) RETURNS TABLE("_ID_" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _ID > 0 THEN
	BEGIN

		DELETE FROM pangya.pangya_golden_time_info WHERE index = _ID;

		DELETE FROM pangya.pangya_golden_time_item WHERE golden_time_id = _ID;

		DELETE FROM pangya.pangya_golden_time_round WHERE golden_time_id = _ID;

		RETURN QUERY SELECT _ID AS ID;

	END;
	ELSE
		RETURN QUERY SELECT -1 AS ID;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procdeletegoldentimeevent(_id bigint) OWNER TO postgres;

--
-- TOC entry 525 (class 1255 OID 23211)
-- Name: procdeletegoldentimeitem(bigint); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procdeletegoldentimeitem(_id bigint) RETURNS TABLE("_ID_" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _ID > 0 THEN
	BEGIN

		DELETE FROM pangya.pangya_golden_time_item WHERE index = _ID;

		RETURN QUERY SELECT _ID AS ID;

	END;
	ELSE
		RETURN QUERY SELECT -1 AS ID;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procdeletegoldentimeitem(_id bigint) OWNER TO postgres;

--
-- TOC entry 526 (class 1255 OID 23212)
-- Name: procdeletegoldentimeround(bigint); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procdeletegoldentimeround(_id bigint) RETURNS TABLE("_ID_" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _ID > 0 THEN
	BEGIN

		DELETE FROM pangya.pangya_golden_time_round WHERE index = _ID;

		RETURN QUERY SELECT _ID AS ID;

	END;
	ELSE
		SELECT -1 AS ID;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procdeletegoldentimeround(_id bigint) OWNER TO postgres;

--
-- TOC entry 527 (class 1255 OID 23213)
-- Name: procdeleteguildbbs(integer, bigint); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procdeleteguildbbs(_owner_uid integer, _bbs_seq bigint) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _BBS_SEQ > 0 THEN
		UPDATE pangya.pangya_guild_bbs SET STATE = 2 WHERE SEQ = _BBS_SEQ AND OWNER_UID = _OWNER_UID;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procdeleteguildbbs(_owner_uid integer, _bbs_seq bigint) OWNER TO postgres;

--
-- TOC entry 528 (class 1255 OID 23214)
-- Name: procdeleteguildbbsreply(integer, bigint); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procdeleteguildbbsreply(_owner_uid integer, _bbs_seq bigint) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _BBS_SEQ > 0 THEN
		UPDATE pangya.pangya_guild_bbs_res SET STATE = 2 WHERE SEQ = _BBS_SEQ AND OWNER_UID = _OWNER_UID;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procdeleteguildbbsreply(_owner_uid integer, _bbs_seq bigint) OWNER TO postgres;

--
-- TOC entry 529 (class 1255 OID 23215)
-- Name: procdeleteguildnotice(integer, bigint); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procdeleteguildnotice(_owner_uid integer, _notice_seq bigint) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _NOTICE_SEQ > 0 THEN
	
		WITH notices as (
			SELECT
				a.GUILD_UID as GUID, b.GUILD_LEADER
			FROM
			pangya.pangya_guild_notice a
			INNER JOIN
			pangya.pangya_guild b
			ON a.GUILD_UID = b.GUILD_UID
		)
		UPDATE pangya.pangya_guild_notice
			SET STATE = 2
		FROM
			notices a
		WHERE a.GUID = GUILD_UID AND SEQ = _NOTICE_SEQ
			AND (_OWNER_UID = OWNER_UID OR a.GUILD_LEADER = _OWNER_UID);
	END IF;
END;
$$;


ALTER FUNCTION pangya.procdeleteguildnotice(_owner_uid integer, _notice_seq bigint) OWNER TO postgres;

--
-- TOC entry 530 (class 1255 OID 23216)
-- Name: procdeleteguildprivatebbs(integer, bigint); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procdeleteguildprivatebbs(_owner_uid integer, _seq bigint) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _SEQ > 0 THEN
		WITH bbs AS (
			SELECT
				GUILD_UID AS GUID, GUILD_LEADER
			FROM
				pangya.pangya_guild
		)
		UPDATE pangya.pangya_guild_private_bbs
			SET STATE = 2
		FROM bbs a
		WHERE GUILD_UID = a.GUID AND SEQ = _SEQ
			AND (OWNER_UID = _OWNER_UID OR a.GUILD_LEADER = _OWNER_UID);
	END IF;
END;
$$;


ALTER FUNCTION pangya.procdeleteguildprivatebbs(_owner_uid integer, _seq bigint) OWNER TO postgres;

--
-- TOC entry 531 (class 1255 OID 23217)
-- Name: procdeleteguildprivatebbsreply(integer, bigint); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procdeleteguildprivatebbsreply(_owner_uid integer, _seq bigint) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _SEQ > 0 THEN
		WITH bbs_res AS (
			SELECT
				b.SEQ AS BBS_SEQ, a.GUILD_LEADER
			FROM
			pangya.pangya_guild a
			INNER JOIN
			pangya.pangya_guild_private_bbs b
			ON a.GUILD_UID = b.GUILD_UID
		)
		UPDATE pangya.pangya_guild_private_bbs_res
			SET STATE = 2
		FROM bbs_res a
		WHERE a.BBS_SEQ = GUILD_BBS_SEQ
			AND SEQ = _SEQ AND (OWNER_UID = _OWNER_UID OR a.GUILD_LEADER = _OWNER_UID);
	END IF;
END;
$$;


ALTER FUNCTION pangya.procdeleteguildprivatebbsreply(_owner_uid integer, _seq bigint) OWNER TO postgres;

--
-- TOC entry 532 (class 1255 OID 23218)
-- Name: procdeleteloginrewardevent(bigint); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procdeleteloginrewardevent(_id bigint) RETURNS TABLE("_ID_" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _ID > 0 THEN
	BEGIN

		DELETE FROM pangya.pangya_login_reward WHERE index = _ID;

		RETURN QUERY SELECT _ID AS ID;

	END;
	ELSE
		RETURN QUERY SELECT -1 AS ID;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procdeleteloginrewardevent(_id bigint) OWNER TO postgres;

--
-- TOC entry 464 (class 1255 OID 23219)
-- Name: procdeletelogrescuepasswordandid(integer, bigint); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procdeletelogrescuepasswordandid(_uid integer, _index bigint) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _UID > 0 AND _INDEX > 0 THEN
		
		DELETE FROM pangya.pangya_rescue_pwd_log WHERE UID = _UID AND index = _INDEX;

	END IF;
END;
$$;


ALTER FUNCTION pangya.procdeletelogrescuepasswordandid(_uid integer, _index bigint) OWNER TO postgres;

--
-- TOC entry 533 (class 1255 OID 23220)
-- Name: procdestribuiitemtutorial(integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procdestribuiitemtutorial(_iduser integer, _valor integer, _opt integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
DECLARE
	_MSGID int;
	_TYPEIDITEM int; 
	_QNTD int;
BEGIN
	INSERT INTO pangya.pangya_gift_table(
		 UID, 
		 fromid, 
		 message, 
		 giftdate, 
		 Lida_YN, 
		 valid, 
		 Flag, 
		 Contador_Vista)
	 VALUES (
		_IDUSER, 
		'@SUPER', 
		'NICE TUTORIAL', 
		now(), 
		0, 
		1, 
		0, 
		0)
	RETURNING msg_id INTO _MSGID;
	
	IF _OPT = 0 THEN
	BEGIN

		IF _VALOR = 1 THEN
		BEGIN

			_TYPEIDITEM = 436207631;

			_QNTD = 3;

		END;
		ELSIF _VALOR = 2 THEN
		BEGIN

			_TYPEIDITEM = 402653191;

			_QNTD = 3;

		END;
		ELSIF _VALOR = 4 THEN
		BEGIN

			_TYPEIDITEM = 402653189;

			_QNTD = 3;

		END;
		ELSIF _VALOR = 8 THEN
		BEGIN

			_TYPEIDITEM = 402653192;

			_QNTD = 3;

		END;
		ELSIF _VALOR = 16 THEN
		BEGIN

			_TYPEIDITEM = 436207632;

			_QNTD = 500;

		END;
		ELSIF _VALOR = 32 THEN
		BEGIN

			_TYPEIDITEM = 402653188;

			_QNTD = 3;

		END;
		ELSIF _VALOR = 64 THEN
		BEGIN

			_TYPEIDITEM = 436207632;

			_QNTD = 500;

		END;
		ELSIF _VALOR = 128 THEN

			_TYPEIDITEM = 436207632;

			_QNTD = 1000;

		END IF;
	END;
	ELSE 
	BEGIN

		IF _VALOR = 256 THEN
		BEGIN

			_TYPEIDITEM = 436207631;

			_QNTD = 3;

		END;
		ELSIF _VALOR = 512 THEN
		BEGIN

			_TYPEIDITEM = 402653224;

			_QNTD = 1;

		END;
		ELSIF _VALOR = 1024 THEN
		BEGIN

			_TYPEIDITEM = 402653190;

			_QNTD = 1;

		END;
		ELSIF _VALOR = 2048 THEN
		BEGIN

			_TYPEIDITEM = 402653191;

			_QNTD = 5;

		END;
		ELSIF _VALOR = 4096 THEN
		BEGIN

			_TYPEIDITEM = 402653184;

			_QNTD = 4;

		END;
		ELSIF _VALOR = 8192 THEN
			_TYPEIDITEM = 402653185;

			_QNTD = 4;

		END IF;
	END;
	END IF;

	perform pangya.ProcInsertItemNoEmail(0, 
		_IDUSER, 
		_MSGID, 
		-1, 
		_TYPEIDITEM, 
		0, 
		_QNTD, 
		0);
END;
$$;


ALTER FUNCTION pangya.procdestribuiitemtutorial(_iduser integer, _valor integer, _opt integer) OWNER TO postgres;

--
-- TOC entry 534 (class 1255 OID 23221)
-- Name: procdo1stanniversary(); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procdo1stanniversary() RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	-- Check if event 1st aniversary was finished
	IF NOT EXISTS (SELECT EVENT_DONE FROM pangya.pangya_1st_anniversary WHERE EVENT_DONE = 1 LIMIT 1) THEN
	DECLARE 
		_UID INT;
		_LOGIN_DAYS INT;
		_CP BIGINT;
		_APT BIGINT = 0;
		_WIN BIGINT = 0;
		_MSG VARCHAR(500);
		_MSGID INT;
		-- Make cursor to all players win CP in 1st aniversary event
		DECLARE BIRTHDAY_EVENT CURSOR FOR
			SELECT 
				a.UID, a.COUNTER 
			FROM 
				pangya.pangya_attendance_reward a
				INNER JOIN
				pangya.user_info b
				ON a.UID = b.UID
			WHERE a.COUNTER >= 10;
	BEGIN

		-- All players apt
		SELECT 
			COUNT(counter) INTO _APT 
		FROM 
			pangya.pangya_attendance_reward a
			INNER JOIN
			pangya.user_info b
			ON a.UID = b.UID;

		OPEN BIRTHDAY_EVENT;
		FETCH NEXT FROM BIRTHDAY_EVENT INTO _UID, _LOGIN_DAYS;
		
		WHILE found LOOP

			_MSGID = 0;
			
			-- CP Insert
			_CP = _LOGIN_DAYS * 10;

			_MSG = CONCAT('First anniversary celebration, You logged ', _LOGIN_DAYS, ' days and won ', _CP, 'CP.');

			INSERT INTO pangya.pangya_gift_table(UID, fromid, message, giftdate, Lida_YN, valid, Flag, Contador_Vista)
			VALUES(_UID, '@SUPERSS', _MSG, now(), 0, 1, 0, 0)
			RETURNING msg_id INTO _MSGID;

			IF _MSGID > 0 THEN
			BEGIN

				-- INSERT Cookie Point to player
				UPDATE pangya.user_info SET Cookie = Cookie + _CP WHERE UID = _UID;

				-- Log
				INSERT INTO pangya.pangya_1st_anniversary_player_win_cp(UID, LOGIN_DAYS, COOKIE_POINT) VALUES(_UID, _LOGIN_DAYS, _CP);

				-- Comando para mostrar no jogo que o player recebeu um presente
				perform pangya.ProcInsertCommand(4, _UID, _MSGID, 0, 0, 0, 1, 0, 1, null);

			END; -- IF MSG IS IS OK
			ELSE
				RAISE NOTICE '%', CONCAT('Failed to make gift table message to player[UID=', _UID, ']');
			END IF;

			-- Count Win player
			_WIN = _WIN + 1;

			-- NEXT ROW
			FETCH NEXT FROM BIRTHDAY_EVENT INTO _UID, _LOGIN_DAYS;

		END LOOP; -- End While

		-- Close cursor
		CLOSE BIRTHDAY_EVENT;
			
		-- Finish Event 1st aniversary
		IF (SELECT EVENT_DONE FROM pangya.pangya_1st_anniversary) IS NULL THEN
			INSERT INTO pangya.pangya_1st_anniversary(EVENT_DONE, ALL_PLAYER_APT, ALL_PLAYER_WIN) VALUES(1, _APT, _WIN);
		ELSE
			UPDATE pangya.pangya_1st_anniversary SET EVENT_DONE = 1, ALL_PLAYER_APT = _APT, ALL_PLAYER_WIN = _WIN;
		END IF;

		RAISE NOTICE '%', 'Event 1s Anniversary was finished succeful';

	END;
	ELSE
	DECLARE
		_MSG_ERR VARCHAR(500);
	BEGIN
		

		SELECT _MSG_ERR = CONCAT('Event 1s Anniversary was finished in ', REG_DATE) FROM pangya.pangya_1st_anniversary LIMIT 1;

		RAISE NOTICE '%', _MSG_ERR;
	END;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procdo1stanniversary() OWNER TO postgres;

--
-- TOC entry 535 (class 1255 OID 23222)
-- Name: procequipcard(integer, integer, integer, integer, integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procequipcard(_iduser integer, _card_typeid integer, _part_id integer, _part_typeid integer, _efeito integer, _efeito_qntd integer, _slot integer, _tipo integer, _tempo integer) RETURNS TABLE("_INDEX_" bigint)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_INDEX BIGINT = -1;
BEGIN
	-- Insert statements for procedure here
	IF _IDUSER > 0 AND _CARD_TYPEID > 0 THEN
	DECLARE
		_REG_DT timestamp = NULL;
		_END_DT timestamp = NULL;
	BEGIN

		IF _TEMPO > 0 THEN
			_REG_DT = now();
			_END_DT = (_REG_DT + ('1 minute'::interval * _TEMPO));
		END IF;

		INSERT INTO pangya.pangya_card_equip(UID, parts_id, parts_typeid, card_typeid, Efeito, Efeito_Qntd, Slot, USE_DT, END_DT, Tipo, USE_YN)
			VALUES(_IDUSER, _PART_ID, _PART_TYPEID, _CARD_TYPEID, _EFEITO, _EFEITO_QNTD, _SLOT, _REG_DT, _END_DT, _TIPO, 1)
		RETURNING index INTO _INDEX;
	END;
	END IF;

	RETURN QUERY SELECT _INDEX AS INDEX;
END;
$$;


ALTER FUNCTION pangya.procequipcard(_iduser integer, _card_typeid integer, _part_id integer, _part_typeid integer, _efeito integer, _efeito_qntd integer, _slot integer, _tipo integer, _tempo integer) OWNER TO postgres;

--
-- TOC entry 536 (class 1255 OID 23223)
-- Name: procexchangecookiepointbyepin(bigint, integer, bigint); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procexchangecookiepointbyepin(_epin_id bigint, _uid integer, _value bigint) RETURNS TABLE("_ID_" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _EPIN_ID > 0 AND _UID > 0 AND _VALUE > 0 THEN
	BEGIN

		-- Update Epin to valid = 0, já trocou o epin e o uid de quem trocou o epin por CP
		UPDATE pangya.pangya_donation_epin
			SET
				retrive_uid = _UID,
				valid = 0
		WHERE
			index = _EPIN_ID;

		-- Adiciona os CPs para o player
		UPDATE pangya.user_info
			SET
				Cookie = Cookie + _VALUE
		WHERE
			UID = _UID;

		-- CP Log
		INSERT INTO pangya.pangya_cookie_point_log(UID, TYPE, mail_id, cookie, item_qnty)
		VALUES(_UID, 4/*ADD CP FROM EPIN DONATION*/, -1, _VALUE, 0);

		RETURN QUERY SELECT _EPIN_ID AS ID;

	END;
	ELSE
		RETURN QUERY SELECT -1 AS ID;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procexchangecookiepointbyepin(_epin_id bigint, _uid integer, _value bigint) OWNER TO postgres;

--
-- TOC entry 537 (class 1255 OID 23224)
-- Name: procexcluimsgmailbox(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procexcluimsgmailbox(_iduser integer, _idmsg integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	UPDATE pangya.pangya_gift_table
         SET 
            valid = 0
      WHERE pangya_gift_table.UID = _IDUSER AND pangya_gift_table.Msg_ID = _IDMSG;
END;
$$;


ALTER FUNCTION pangya.procexcluimsgmailbox(_iduser integer, _idmsg integer) OWNER TO postgres;

--
-- TOC entry 538 (class 1255 OID 23225)
-- Name: procextendediascaddie(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procextendediascaddie(_iduser integer, _caddie_id integer) RETURNS TABLE("_TYPEID_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	UPDATE pangya.pangya_caddie_information
		SET 
			EndDate = (now() + '30 day'::interval)
	WHERE pangya_caddie_information.UID = _IDUSER AND pangya_caddie_information.item_id = _Caddie_ID;

	RETURN QUERY SELECT pangya_caddie_information.typeid
	FROM pangya.pangya_caddie_information
	WHERE pangya_caddie_information.UID = _IDUSER AND pangya_caddie_information.item_id = _caddie_id;
END;
$$;


ALTER FUNCTION pangya.procextendediascaddie(_iduser integer, _caddie_id integer) OWNER TO postgres;

--
-- TOC entry 539 (class 1255 OID 23226)
-- Name: procextendrental(integer, integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procextendrental(_iduser integer, _iditem integer, _end_date text) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	UPDATE pangya.pangya_item_warehouse SET EndDate = _END_DATE::timestamp WHERE UID = _IDUSER AND item_id = _IDITEM;
END;
$$;


ALTER FUNCTION pangya.procextendrental(_iduser integer, _iditem integer, _end_date text) OWNER TO postgres;

--
-- TOC entry 540 (class 1255 OID 23227)
-- Name: procfindcaddie(bigint, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procfindcaddie(_iduser bigint, _itemtypeid integer) RETURNS TABLE(_item_id_ bigint, "_UID_" integer, _typeid_ integer, _parts_typeid_ integer, "_cLevel_" smallint, "_Exp_" integer, "_RentFlag_" smallint, "_EndDate_" timestamp without time zone, "_Purchase_" smallint, "_parts_EndDate_" timestamp without time zone, "_CheckEnd_" smallint)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_ITEM_ID BIGINT = (SELECT item_id FROM pangya.pangya_caddie_information WHERE UID = _IDUSER AND typeid = _ITEMTYPEID AND valid = 1 LIMIT 1);
BEGIN
	IF _ITEM_ID IS NOT NULL THEN
		RETURN QUERY SELECT * FROM pangya.ProcGetCaddieInfo_One(_IDUSER::int, _ITEM_ID::int);
	ELSE
		RETURN QUERY SELECT -1::bigint AS item_id,
				0::int as uid,
				0::int as typeid,
				0::int as parts_typeid,
				0::smallint as clevel,
				0::int as exp,
				0::smallint as RentFlag,
				null::timestamp as EndDate,
				0::smallint as Purchase,
				null::timestamp as parts_EndDate,
				0::smallint CheckEnd;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procfindcaddie(_iduser bigint, _itemtypeid integer) OWNER TO postgres;

--
-- TOC entry 541 (class 1255 OID 23228)
-- Name: procfindcard(bigint, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procfindcard(_iduser bigint, _itemtypeid integer) RETURNS TABLE(_card_itemid_ bigint, "_UID_" integer, _card_typeid_ integer, "_Slot_" integer, "_Efeito_" integer, "_Efeito_Qntd_" integer, "_QNTD_" integer, "_USE_DT_" timestamp without time zone, "_END_DT_" timestamp without time zone, _card_type_ smallint, "_USE_YN_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_ITEM_ID BIGINT = (SELECT card_itemid FROM pangya.pangya_card WHERE QNTD > 0 AND UID = _IDUSER AND card_typeid = _ITEMTYPEID LIMIT 1);
BEGIN
	IF _ITEM_ID IS NOT NULL THEN
		RETURN QUERY SELECT * FROM pangya.ProcGetCardInfo_One(_IDUSER, _ITEM_ID);
	ELSE
		RETURN QUERY SELECT -1::bigint AS item_id,
				0::int as uid,
				0::int as card_typeid,
				0::int as slot,
				0::int as Efeito,
				0::int as Efeito_Qntd,
				0::int as qntd,
				null::timestamp as use_dt,
				null::timestamp as end_dt,
				0::smallint as card_type,
				0::int as use_yn;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procfindcard(_iduser bigint, _itemtypeid integer) OWNER TO postgres;

--
-- TOC entry 542 (class 1255 OID 23229)
-- Name: procfindcharacter(bigint, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procfindcharacter(_iduser bigint, _itemtypeid integer) RETURNS TABLE(_item_id_ bigint, _typeid_ integer, "_ITEMID_PARTS_1_" integer, "_ITEMID_PARTS_2_" integer, "_ITEMID_PARTS_3_" integer, "_ITEMID_PARTS_4_" integer, "_ITEMID_PARTS_5_" integer, "_ITEMID_PARTS_6_" integer, "_ITEMID_PARTS_7_" integer, "_ITEMID_PARTS_8_" integer, "_ITEMID_PARTS_9_" integer, "_ITEMID_PARTS_10_" integer, "_ITEMID_PARTS_11_" integer, "_ITEMID_PARTS_12_" integer, "_ITEMID_PARTS_13_" integer, "_ITEMID_PARTS_14_" integer, "_ITEMID_PARTS_15_" integer, "_ITEMID_PARTS_16_" integer, "_ITEMID_PARTS_17_" integer, "_ITEMID_PARTS_18_" integer, "_ITEMID_PARTS_19_" integer, "_ITEMID_PARTS_20_" integer, "_ITEMID_PARTS_21_" integer, "_ITEMID_PARTS_22_" integer, "_ITEMID_PARTS_23_" integer, "_ITEMID_PARTS_24_" integer, _parts_1_ integer, _parts_2_ integer, _parts_3_ integer, _parts_4_ integer, _parts_5_ integer, _parts_6_ integer, _parts_7_ integer, _parts_8_ integer, _parts_9_ integer, _parts_10_ integer, _parts_11_ integer, _parts_12_ integer, _parts_13_ integer, _parts_14_ integer, _parts_15_ integer, _parts_16_ integer, _parts_17_ integer, _parts_18_ integer, _parts_19_ integer, _parts_20_ integer, _parts_21_ integer, _parts_22_ integer, _parts_23_ integer, _parts_24_ integer, _default_hair_ smallint, _default_shirts_ smallint, _gift_flag_ smallint, "_PCL0_" smallint, "_PCL1_" smallint, "_PCL2_" smallint, "_PCL3_" smallint, "_PCL4_" smallint, "_Purchase_" smallint, _auxparts_1_ integer, _auxparts_2_ integer, _auxparts_3_ integer, _auxparts_4_ integer, _auxparts_5_ integer, "_CutIn_1_" integer, "_CutIn_2_" integer, "_CutIn_3_" integer, "_CutIn_4_" integer, "_Mastery_" integer, "_CARD_CHARACTER_1_" integer, "_CARD_CHARACTER_2_" integer, "_CARD_CHARACTER_3_" integer, "_CARD_CHARACTER_4_" integer, "_CARD_CADDIE_1_" integer, "_CARD_CADDIE_2_" integer, "_CARD_CADDIE_3_" integer, "_CARD_CADDIE_4_" integer, "_CARD_NPC_1_" integer, "_CARD_NPC_2_" integer, "_CARD_NPC_3_" integer, "_CARD_NPC_4_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_ITEM_ID BIGINT = (SELECT item_id FROM pangya.pangya_character_information WHERE UID = _IDUSER AND typeid = _ITEMTYPEID LIMIT 1);
BEGIN
	IF _ITEM_ID IS NOT NULL THEN
		RETURN QUERY SELECT * FROM pangya.USP_CHAR_EQUIP_LOAD_S4_ONE(_IDUSER, _ITEM_ID);
	ELSE
		RETURN QUERY SELECT -1::bigint AS item_id, 
            0::int as typeid, 
            0::int as ITEMID_PARTS_1, 
            0::int as ITEMID_PARTS_2, 
            0::int as ITEMID_PARTS_3, 
            0::int as ITEMID_PARTS_4, 
            0::int as ITEMID_PARTS_5, 
            0::int as ITEMID_PARTS_6, 
            0::int as ITEMID_PARTS_7, 
            0::int as ITEMID_PARTS_8, 
            0::int as ITEMID_PARTS_9, 
            0::int as ITEMID_PARTS_10, 
            0::int as ITEMID_PARTS_11, 
            0::int as ITEMID_PARTS_12, 
            0::int as ITEMID_PARTS_13, 
            0::int as ITEMID_PARTS_14, 
            0::int as ITEMID_PARTS_15, 
            0::int as ITEMID_PARTS_16, 
            0::int as ITEMID_PARTS_17, 
            0::int as ITEMID_PARTS_18, 
            0::int as ITEMID_PARTS_19, 
            0::int as ITEMID_PARTS_20, 
            0::int as ITEMID_PARTS_21, 
            0::int as ITEMID_PARTS_22, 
            0::int as ITEMID_PARTS_23, 
            0::int as ITEMID_PARTS_24, 
            0::int as parts_1, 
            0::int as parts_2, 
            0::int as parts_3, 
            0::int as parts_4, 
            0::int as parts_5, 
            0::int as parts_6, 
            0::int as parts_7, 
            0::int as parts_8, 
            0::int as parts_9, 
            0::int as parts_10, 
            0::int as parts_11, 
            0::int as parts_12, 
            0::int as parts_13, 
            0::int as parts_14, 
            0::int as parts_15, 
            0::int as parts_16, 
            0::int as parts_17, 
            0::int as parts_18, 
            0::int as parts_19, 
            0::int as parts_20, 
            0::int as parts_21, 
            0::int as parts_22, 
            0::int as parts_23, 
            0::int as parts_24, 
            0::smallint as default_hair, 
            0::smallint as default_shirts, 
            0::smallint as gift_flag, 
            0::smallint as PCL0, 
            0::smallint as PCL1, 
            0::smallint as PCL2, 
            0::smallint as PCL3, 
            0::smallint as PCL4, 
            0::smallint as Purchase, 
            0::int as auxparts_1, 
            0::int as auxparts_2, 
            0::int as auxparts_3, 
            0::int as auxparts_4, 
            0::int as auxparts_5, 
            0::int as CutIn_1,
            0::int as CutIn_2,
            0::int as CutIn_3,
            0::int as CutIn_4,
            0::int as Mastery, 
            0::int as CARD_CHARACTER_1, 
            0::int as CARD_CHARACTER_2, 
            0::int as CARD_CHARACTER_3, 
            0::int as CARD_CHARACTER_4, 
            0::int as CARD_CADDIE_1, 
            0::int as CARD_CADDIE_2, 
            0::int as CARD_CADDIE_3, 
            0::int as CARD_CADDIE_4, 
            0::int as CARD_NPC_1, 
            0::int as CARD_NPC_2, 
            0::int as CARD_NPC_3, 
            0::int as CARD_NPC_4;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procfindcharacter(_iduser bigint, _itemtypeid integer) OWNER TO postgres;

--
-- TOC entry 543 (class 1255 OID 23230)
-- Name: procfinddolfinilockeritem(bigint, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procfinddolfinilockeritem(_iduser bigint, _itemtypeid integer) RETURNS TABLE(_item_id_ bigint, _uid_ integer, _typeid_ integer, "_SD_NAME_" character varying, "_SD_IDX_" character varying, "_SD_SEQ_" integer, "_SD_COPIER_NICK_" character varying, "_SD_STATUS_" integer, "_IDX_DOLFINI_LOCKER_ITEM_" bigint, "_FLAG_DOLFINI_LOCKER_ITEM_" smallint)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_ITEM_ID BIGINT = (SELECT a.item_id FROM pangya.pangya_item_warehouse a INNER JOIN pangya.pangya_dolfini_locker_item b ON a.item_id = b.item_id WHERE a.UID = _IDUSER AND b.flag = 1 AND a.typeid = _ITEMTYPEID LIMIT 1);
BEGIN
	IF _ITEM_ID IS NOT NULL THEN
		RETURN QUERY SELECT * FROM pangya.ProcGetDolfiniLockerItemOne(_IDUSER, _ITEM_ID);
	ELSE
		RETURN QUERY SELECT -1::bigint AS item_id,
				0::int,
				0::int,
				null::varchar,
				null::varchar,
				0::int,
				null::varchar,
				0::int,
				0::bigint,
				0::smallint;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procfinddolfinilockeritem(_iduser bigint, _itemtypeid integer) OWNER TO postgres;

--
-- TOC entry 544 (class 1255 OID 23231)
-- Name: procfindfurniture(bigint, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procfindfurniture(_iduser bigint, _itemtypeid integer) RETURNS TABLE("_MYROOM_ID_" bigint, "_UID_" integer, "_TYPEID_" integer, "_ROOM_NO_" integer, "_POS_X_" real, "_POS_Y_" real, "_POS_Z_" real, "_POS_R_" real, "_USE_YN_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_ITEM_ID BIGINT = (SELECT MYROOM_ID FROM pangya.td_room_data WHERE UID = _IDUSER AND valid = 1 AND TYPEID = _ITEMTYPEID LIMIT 1);
BEGIN
	IF _ITEM_ID IS NOT NULL THEN
		RETURN QUERY SELECT * FROM pangya.ProcGetMyRoom_One(_IDUSER, _ITEM_ID);
	ELSE
		RETURN QUERY SELECT -1::bigint AS item_id,
				0::int as uid,
				0::int as typeid,
				0::int as room_no,
				0::real as pos_x,
				0::real as pos_y,
				0::real as pos_z,
				0::real as pos_r,
				0::int as use_yn;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procfindfurniture(_iduser bigint, _itemtypeid integer) OWNER TO postgres;

--
-- TOC entry 545 (class 1255 OID 23232)
-- Name: procfindmailboxitem(bigint, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procfindmailboxitem(_iduser bigint, _itemtypeid integer) RETURNS TABLE("_ITEM_TYPEID_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_ITEM_TYPEID INT = (SELECT b.item_typeid 
						FROM pangya.pangya_gift_table a 
						INNER JOIN pangya.pangya_item_mail b 
						ON a.Msg_ID = b.Msg_ID 
						WHERE a.UID = _IDUSER AND a.valid = 1 AND b.valid = 1 AND b.item_typeid = _ITEMTYPEID 
						LIMIT 1);
BEGIN
	
	IF _ITEM_TYPEID IS NOT NULL THEN 
		RETURN QUERY SELECT _ITEM_TYPEID AS ITEM_TYPEID;
	ELSE 
		RETURN QUERY SELECT -1::int AS ITEM_TYPEID;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procfindmailboxitem(_iduser bigint, _itemtypeid integer) OWNER TO postgres;

--
-- TOC entry 546 (class 1255 OID 23233)
-- Name: procfindmascot(bigint, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procfindmascot(_iduser bigint, _itemtypeid integer) RETURNS TABLE(_item_id_ bigint, "_UID_" integer, _typeid_ integer, "_mLevel_" smallint, "_mExp_" integer, "_Flag_" smallint, "_Message_" character varying, "_Tipo_" smallint, "_IsCash_" smallint, "_EndDate_" timestamp without time zone)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_ITEM_ID BIGINT = (SELECT item_id FROM pangya.pangya_mascot_info WHERE valid = 1 AND UID = _IDUSER AND typeid = _ITEMTYPEID AND (CASE WHEN tipo = 0 OR enddate > now() THEN 1 ELSE 0 END) <> 0 LIMIT 1);
BEGIN
	IF _ITEM_ID IS NOT NULL THEN
		RETURN QUERY SELECT * FROM pangya.ProcGetMascotInfo_One(_IDUSER, _ITEM_ID);
	ELSE
		RETURN QUERY SELECT -1::bigint AS ITEM_ID,
				0::int as uid,
				0::int as typeid,
				0::smallint as mLevel,
				0::int as mExp,
				0::smallint as flag,
				null::varchar as message,
				0::smallint as tipo,
				0::smallint as IsCash,
				null::timestamp as EndDate;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procfindmascot(_iduser bigint, _itemtypeid integer) OWNER TO postgres;

--
-- TOC entry 547 (class 1255 OID 23234)
-- Name: procfindtrofelgrandprix(bigint, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procfindtrofelgrandprix(_iduser bigint, _itemtypeid integer) RETURNS TABLE(_item_id_ bigint, _typeid_ integer, _qntd_ integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_ITEM_ID BIGINT = (SELECT item_id FROM pangya.pangya_trofel_grandprix WHERE UID = _IDUSER AND typeid = _ITEMTYPEID LIMIT 1);
BEGIN
	IF _ITEM_ID IS NOT NULL THEN
		RETURN QUERY SELECT item_id, typeid, qntd FROM pangya.pangya_trofel_grandprix WHERE UID = _IDUSER AND item_id = _ITEM_ID;
	ELSE
		RETURN QUERY SELECT -1::bigint AS ITEM_ID,
				0::int as typeid,
				0::int as qntd;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procfindtrofelgrandprix(_iduser bigint, _itemtypeid integer) OWNER TO postgres;

--
-- TOC entry 548 (class 1255 OID 23235)
-- Name: procfindtrofelspecial(bigint, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procfindtrofelspecial(_iduser bigint, _itemtypeid integer) RETURNS TABLE(_item_id_ bigint, _typeid_ integer, _qntd_ integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_ITEM_ID BIGINT = (SELECT item_id FROM pangya.pangya_trofel_especial WHERE UID = _IDUSER AND typeid = _ITEMTYPEID LIMIT 1);
BEGIN
	IF _ITEM_ID IS NOT NULL THEN
		RETURN QUERY SELECT item_id, typeid, qntd FROM pangya.pangya_trofel_especial WHERE UID = _IDUSER AND item_id = _ITEM_ID;
	ELSE
		RETURN QUERY SELECT -1::bigint AS ITEM_ID,
					0::int as typeid,
					0::int as qntd;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procfindtrofelspecial(_iduser bigint, _itemtypeid integer) OWNER TO postgres;

--
-- TOC entry 549 (class 1255 OID 23236)
-- Name: procfinducc(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procfinducc(_ucc_id integer) RETURNS TABLE(_item_id_ bigint, "_UID_" integer, _typeid_ integer, "_Ano_" integer, "_C0_" smallint, "_C1_" smallint, "_C2_" smallint, "_C3_" smallint, "_C4_" smallint, "_Purchase_" smallint, "_Gift_flag_" smallint, "_Flag_" integer, "_APPLY_DATE_" bigint, "_END_DATE_" bigint, "_ItemType_" smallint, "_CARD_CHARACTER_1_" integer, "_CARD_CHARACTER_2_" integer, "_CARD_CHARACTER_3_" integer, "_CARD_CHARACTER_4_" integer, "_CARD_CADDIE_1_" integer, "_CARD_CADDIE_2_" integer, "_CARD_CADDIE_3_" integer, "_CARD_CADDIE_4_" integer, "_CARD_NPC_1_" integer, "_CARD_NPC_2_" integer, "_CARD_NPC_3_" integer, "_CARD_NPC_4_" integer, "_ClubSet_WorkShop_Flag_" smallint, "_ClubSet_WorkShop_C0_" smallint, "_ClubSet_WorkShop_C1_" smallint, "_ClubSet_WorkShop_C2_" smallint, "_ClubSet_WorkShop_C3_" smallint, "_ClubSet_WorkShop_C4_" smallint, "_Mastery_Pts_" integer, "_Recovery_Pts_" integer, "_Level_" integer, "_Up_" integer, "_SD_NAME_" character varying, "_SD_IDX_" character varying, "_SD_SEQ_" integer, "_SD_COPIER_NICK_" character varying, "_SD_COPIER_" integer, "_SD_TRADE_" integer, "_SD_FLAG_" integer, "_SD_STATUS_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _UCC_ID > 0 THEN
	DECLARE 
		_IDUSER INT = (SELECT uid FROM pangya.pangya_item_warehouse WHERE valid = 1 AND Gift_Flag = 0 AND item_id = _UCC_ID LIMIT 1);
	BEGIN

		IF _IDUSER IS NOT NULL AND _IDUSER > 0
			AND (SELECT COPIER FROM pangya.tu_ucc WHERE UID = _IDUSER AND ITEM_ID = _UCC_ID) IS NOT NULL THEN
 		BEGIN
	
			RETURN QUERY SELECT * FROM pangya.ProcGetWareHouseItem_One(_IDUSER, _UCC_ID);
		END;
		ELSE
			RETURN QUERY SELECT -1::bigint AS ITEM_ID, 
				0::int,
				0::int,
				0::int,
				0::smallint,
				0::smallint,
				0::smallint,
				0::smallint,
				0::smallint,
				0::smallint,
				0::smallint,
				0::int,
				0::bigint,
				0::bigint,
				0::smallint,
				0::int,
				0::int,
				0::int,
				0::int,
				0::int,
				0::int,
				0::int,
				0::int,
				0::int,
				0::int,
				0::int,
				0::int,
				0::smallint,
				0::smallint,
				0::smallint,
				0::smallint,
				0::smallint,
				0::smallint,
				0::int,
				0::int,
				0::int,
				0::int,
				null::varchar,
				null::varchar,
				0::int,
				null::varchar,
				0::int,
				0::int,
				0::int,
				0::int;
		END IF;
	END;
	ELSE
		RETURN QUERY SELECT -1 AS ITEM_ID,
				0::int,
				0::int,
				0::int,
				0::smallint,
				0::smallint,
				0::smallint,
				0::smallint,
				0::smallint,
				0::smallint,
				0::smallint,
				0::int,
				0::bigint,
				0::bigint,
				0::smallint,
				0::int,
				0::int,
				0::int,
				0::int,
				0::int,
				0::int,
				0::int,
				0::int,
				0::int,
				0::int,
				0::int,
				0::int,
				0::smallint,
				0::smallint,
				0::smallint,
				0::smallint,
				0::smallint,
				0::smallint,
				0::int,
				0::int,
				0::int,
				0::int,
				null::varchar,
				null::varchar,
				0::int,
				null::varchar,
				0::int,
				0::int,
				0::int,
				0::int;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procfinducc(_ucc_id integer) OWNER TO postgres;

--
-- TOC entry 550 (class 1255 OID 23237)
-- Name: procfindwarehouseitem(bigint, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procfindwarehouseitem(_iduser bigint, _itemtypeid integer) RETURNS TABLE(_item_id_ bigint, "_UID_" integer, _typeid_ integer, "_Ano_" integer, "_C0_" smallint, "_C1_" smallint, "_C2_" smallint, "_C3_" smallint, "_C4_" smallint, "_Purchase_" smallint, "_Gift_flag_" smallint, "_Flag_" integer, "_APPLY_DATE_" bigint, "_END_DATE_" bigint, "_ItemType_" smallint, "_CARD_CHARACTER_1_" integer, "_CARD_CHARACTER_2_" integer, "_CARD_CHARACTER_3_" integer, "_CARD_CHARACTER_4_" integer, "_CARD_CADDIE_1_" integer, "_CARD_CADDIE_2_" integer, "_CARD_CADDIE_3_" integer, "_CARD_CADDIE_4_" integer, "_CARD_NPC_1_" integer, "_CARD_NPC_2_" integer, "_CARD_NPC_3_" integer, "_CARD_NPC_4_" integer, "_ClubSet_WorkShop_Flag_" smallint, "_ClubSet_WorkShop_C0_" smallint, "_ClubSet_WorkShop_C1_" smallint, "_ClubSet_WorkShop_C2_" smallint, "_ClubSet_WorkShop_C3_" smallint, "_ClubSet_WorkShop_C4_" smallint, "_Mastery_Pts_" integer, "_Recovery_Pts_" integer, "_Level_" integer, "_Up_" integer, "_SD_NAME_" character varying, "_SD_IDX_" character varying, "_SD_SEQ_" integer, "_SD_COPIER_NICK_" character varying, "_SD_COPIER_" integer, "_SD_TRADE_" integer, "_SD_FLAG_" integer, "_SD_STATUS_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_ITEM_ID BIGINT = (SELECT item_id FROM pangya.pangya_item_warehouse WHERE valid = 1 AND Gift_Flag = 0 AND UID = _IDUSER AND typeid = _ITEMTYPEID LIMIT 1);
BEGIN
	IF _ITEM_ID IS NOT NULL THEN
		RETURN QUERY SELECT * FROM pangya.ProcGetWareHouseItem_One(_IDUSER, _ITEM_ID);
	ELSE
		RETURN QUERY SELECT -1::bigint AS ITEM_ID,
				0::int as UID, 
				0::int as typeid, 
				0::int as Ano, 
				0::smallint as C0, 
				0::smallint as C1, 
				0::smallint as C2, 
				0::smallint as C3, 
				0::smallint as C4, 
				0::smallint as Purchase, 
				0::smallint as Gift_flag, 
				0::int as Flag, 
				0::bigint as APPLY_DATE, 
				0::bigint as END_DATE, 
				0::smallint as ItemType, 
				0::int as CARD_CHARACTER_1, 
				0::int as CARD_CHARACTER_2, 
				0::int as CARD_CHARACTER_3, 
				0::int as CARD_CHARACTER_4, 
				0::int as CARD_CADDIE_1, 
				0::int as CARD_CADDIE_2, 
				0::int as CARD_CADDIE_3, 
				0::int as CARD_CADDIE_4, 
				0::int as CARD_NPC_1, 
				0::int as CARD_NPC_2, 
				0::int as CARD_NPC_3, 
				0::int as CARD_NPC_4, 
				0::smallint as ClubSet_WorkShop_Flag, 
				0::smallint as ClubSet_WorkShop_C0, 
				0::smallint as ClubSet_WorkShop_C1, 
				0::smallint as ClubSet_WorkShop_C2, 
				0::smallint as ClubSet_WorkShop_C3, 
				0::smallint as ClubSet_WorkShop_C4, 
				0::int as Mastery_Pts, 
				0::int as Recovery_Pts, 
				0::int as Level, 
				0::int as Up, 
				null::varchar as SD_NAME, 
				null::varchar as SD_IDX, 
				0::int as SD_SEQ, 
				null::varchar as SD_COPIER_NICK, 
				0::int as SD_COPIER, 
				0::int as SD_TRADE, 
				0::int as SD_FLAG, 
				0::int as SD_STATUS;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procfindwarehouseitem(_iduser bigint, _itemtypeid integer) OWNER TO postgres;

--
-- TOC entry 551 (class 1255 OID 23238)
-- Name: procfinishluciaattendancereward(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procfinishluciaattendancereward(_uid integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _UID > 0 THEN
		UPDATE pangya.pangya_lucia_attendance
			SET last_day_get_item = now(),
				last_day_attendance = now(),
				count_day = 0
		WHERE UID = _UID;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procfinishluciaattendancereward(_uid integer) OWNER TO postgres;

--
-- TOC entry 552 (class 1255 OID 23239)
-- Name: procfinishquestuser(bigint, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procfinishquestuser(_iduser bigint, _id integer) RETURNS TABLE("_DATE_" bigint)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_DT_NOW timestamp = now();
BEGIN
	UPDATE pangya.pangya_quest SET Date = _DT_NOW WHERE UID = _IDUSER AND id = _ID;
	
	RETURN QUERY SELECT public."UNIX_TIMESTAMP"(_DT_NOW) AS Date;
END;
$$;


ALTER FUNCTION pangya.procfinishquestuser(_iduser bigint, _id integer) OWNER TO postgres;

--
-- TOC entry 553 (class 1255 OID 23240)
-- Name: procfirstset(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procfirstset(_iduser integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
DECLARE _ITEMID INT DEFAULT 0;
BEGIN

    -- Club CV1 0x10000000
	--exec USP_ADD_ITEM @IDUSER, 0, 0, -1, 268435456, 0, 0, 0, 8, 9, 8, 3, 3, 0, 0, 0, 0 
	perform pangya.USP_ADD_ITEM(_IDUSER, 0, 0, -1, 268435456, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	
	SELECT item_id INTO _ITEMID FROM pangya.pangya_item_warehouse WHERE UID = _IDUSER AND typeid = 268435456;

	UPDATE pangya.pangya_user_equip SET club_id = _ITEMID WHERE UID = _IDUSER;

	-- Ball Padrão 0x14000000
	perform pangya.USP_ADD_ITEM(_IDUSER, 0, 0, -1, 335544320, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

	UPDATE pangya.pangya_user_equip SET ball_type = 335544320 WHERE UID = _IDUSER;

	-- Item Padrão que da pixel para os rookie
	perform pangya.USP_ADD_ITEM(_IDUSER, 0, 0, -1, 436207622, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0);
    
    -- MP, Mileage Points Tiki Shop
	perform pangya.USP_ADD_ITEM(_IDUSER, 0, 0, -1, 436208295, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0);
    
    -- Assist Item, para o modo assistente
	perform pangya.USP_ADD_ITEM(_IDUSER, 0, 0, -1, 467664918, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0);
    
    -- Ativa Assistente no banco de dados
    UPDATE pangya.pangya_assistente SET assist = 1 WHERE UID = _IDUSER;
    
    -- 30 Grand Prix Ticket para jogar Grand Prix
    perform pangya.USP_ADD_ITEM(_IDUSER, 0, 0, -1, x'1A000264'::int, 0, 0, 0, 30, 0, 0, 0, 0, 0, 0, 0, 0);

	-- User info, ESSES VALORES É SÓ PARA O CLOSED BETA, TROCA ISSO DEPOIS PARA O NORMAL
    UPDATE pangya.user_info SET Pang = Pang + 1000000, Cookie = Cookie + 90 WHERE UID = _IDUSER;
	
	-- Seta o Primeiro Set do Usuario
	UPDATE pangya.account SET FIRST_SET = 1, Event = 1 WHERE UID = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.procfirstset(_iduser integer) OWNER TO postgres;

--
-- TOC entry 555 (class 1255 OID 23241)
-- Name: procgachacheckplayer(text, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgachacheckplayer(_id_in text, _pass text) RETURNS TABLE("_UID_" bigint, "__KEY_" character varying)
    LANGUAGE plpgsql
    AS $_$
DECLARE 
	_IDUSER INT;
	_returnvalue INT;
BEGIN
	SELECT returnvalue INTO _returnvalue FROM pangya.CheckWeblink_Key$IMPL(_id_in, _pass);

	_IDUSER = _returnvalue;
    
	IF _IDUSER >= 0 THEN
	DECLARE
		_KEY_ VARCHAR(22);
		_coin_count INT;
	BEGIN

		_KEY_ = upper(left(md5(random()::text), 22));

        _coin_count = COALESCE((SELECT COUNT(c0) as c0 FROM pangya.pangya_item_warehouse WHERE UID = _IDUSER AND typeid = 436207832), 0);
        
		IF (SELECT UID FROM pangya.pangya_gacha_user_key WHERE UID = _IDUSER) IS NULL THEN --# Insere na tabela
			INSERT INTO pangya_gacha_user_key(uid, coin_count_entrou, key) VALUES(_IDUSER, _coin_count, _KEY_);
        ELSE --# Att a Key
			UPDATE pangya.pangya_gacha_user_key SET att_flag = 0, coin_count_entrou = _coin_count, key = _KEY_, date_key_generation = now() WHERE uid = _IDUSER;
		END IF;
        
        RETURN QUERY SELECT _IDUSER as UID, _KEY_ as KEY;

	END;
	ELSE
		RETURN QUERY SELECT -1 as UID, null::varchar(22);
	END IF;
END;
$_$;


ALTER FUNCTION pangya.procgachacheckplayer(_id_in text, _pass text) OWNER TO postgres;

--
-- TOC entry 556 (class 1255 OID 23242)
-- Name: procgeraauthkeygame(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgeraauthkeygame(_iduser integer, _serverid integer) RETURNS TABLE("_AUTH_KEY_" character varying)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_Auth_Key varchar(7);
	_COUNTER int;
BEGIN
	SELECT left(upper(md5(random()::text)), 7) INTO _Auth_Key;

    SELECT count(*) INTO _COUNTER
    FROM pangya.authkey_game
    WHERE UID = _IDUSER AND ServerID = _SERVERID;

    IF _COUNTER > 0 THEN
        UPDATE pangya.authkey_game
            SET 
                AuthKey = _Auth_Key, 
                valid = 1
        WHERE UID = _IDUSER AND ServerID = _SERVERID;
    ELSE 
        INSERT INTO pangya.authkey_game(UID, AuthKey, ServerID, valid)
            VALUES (_IDUSER, _Auth_Key, _SERVERID, 1);
    END IF;

    RETURN QUERY SELECT AuthKey
    FROM pangya.authkey_game
    WHERE UID = _IDUSER AND ServerID = _SERVERID;
END;
$$;


ALTER FUNCTION pangya.procgeraauthkeygame(_iduser integer, _serverid integer) OWNER TO postgres;

--
-- TOC entry 557 (class 1255 OID 23243)
-- Name: procgeraauthkeylogin(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgeraauthkeylogin(_iduser integer) RETURNS TABLE("_AUTH_KEY_" character varying)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_Auth_Key varchar(7);
	_COUNTER int default 0;
BEGIN
	SELECT left(upper(md5(random()::text)), 7) INTO _Auth_Key;

    SELECT count(*) INTO _COUNTER
    FROM pangya.authkey_login
    WHERE UID = _IDUSER;

    IF _COUNTER > 0 THEN
        UPDATE pangya.authkey_login
            SET 
                AuthKey = _Auth_Key, 
                valid = 1
        WHERE UID = _IDUSER;
    ELSE 
        INSERT INTO pangya.authkey_login(UID, AuthKey, valid)
            VALUES (_IDUSER, _Auth_Key, 1);
    END IF;

    RETURN QUERY SELECT AuthKey
    FROM pangya.authkey_login
    WHERE UID = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.procgeraauthkeylogin(_iduser integer) OWNER TO postgres;

--
-- TOC entry 558 (class 1255 OID 23244)
-- Name: procgerasecuritykey(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgerasecuritykey(_iduser integer, _iditem integer) RETURNS TABLE("_S_KEY_" character varying)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_S_KEY varchar(20);
BEGIN
	SELECT upper(left(md5(random()::text), 20)) INTO _S_KEY;

      UPDATE pangya.tu_ucc
         SET 
            SKEY = _S_KEY
      WHERE tu_ucc.UID = _IDUSER AND tu_ucc.ITEM_ID = _IDITEM;

      RETURN QUERY SELECT _S_KEY AS S_KEY;
END;
$$;


ALTER FUNCTION pangya.procgerasecuritykey(_iduser integer, _iditem integer) OWNER TO postgres;

--
-- TOC entry 559 (class 1255 OID 23245)
-- Name: procgeraweblinkkey(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgeraweblinkkey(_iduser integer) RETURNS TABLE("_WEBLINK_KEY_" character varying)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_WEBLINK_KEY VARCHAR(6);
BEGIN
	SELECT upper(left(md5(random()::text), 6)) INTO _WEBLINK_KEY;
    
    IF (SELECT uid FROM pangya.pangya_weblink_key WHERE UID = _IDUSER) IS NOT NULL THEN
		UPDATE pangya.pangya_weblink_key SET key = _WEBLINK_KEY, valid = 1 WHERE UID = _IDUSER;
    ELSE
		INSERT INTO pangya.pangya_weblink_key(uid, key, valid) VALUES(_IDUSER, _WEBLINK_KEY, 1);
	END IF;
    
    RETURN QUERY SELECT key FROM pangya.pangya_weblink_key WHERE UID = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.procgeraweblinkkey(_iduser integer) OWNER TO postgres;

--
-- TOC entry 560 (class 1255 OID 23246)
-- Name: procgetachievement_new(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetachievement_new(_iduser integer) RETURNS TABLE("_ACHIEVEMENT_TIPO_" smallint, "_ACHIEVEMENT_TYPEID_" integer, "_ACHIEVEMENT_ID_" bigint, "_ACHIEVEMENT_OPTION_" integer, "_ACHIEVEMENT_QUEST_TYPEID_" integer, "_ACHIEVEMENT_COUNTER_TYPEID_" integer, "_ACHIEVEMENT_COUNTER_ID_" integer, "_Count_Num_Item_" bigint, "_ACHIEVEMENT_COUNTER_DATA_SEC_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT 
         Z.TIPO AS ACHIEVEMENT_TIPO, 
         Z.TypeID AS ACHIEVEMENT_TYPEID, 
         Z.ID_ACHIEVEMENT AS ACHIEVEMENT_ID, 
         Z.Option AS ACHIEVEMENT_OPTION, 
         A.TypeID_ACHIEVE AS ACHIEVEMENT_QUEST_TYPEID, 
         A.COUNT_TYPEID AS ACHIEVEMENT_COUNTER_TYPEID, 
         A.Count_ID AS ACHIEVEMENT_COUNTER_ID, 
         A.Count_Num_Item, 
         A.Data_Sec AS ACHIEVEMENT_COUNTER_DATA_SEC
      FROM 
         (
            SELECT achievement_tipo.TypeID, achievement_tipo.ID_ACHIEVEMENT, achievement_tipo.TIPO, achievement_tipo.Option
            FROM pangya.achievement_tipo
            WHERE achievement_tipo.UID = _IDUSER AND achievement_tipo.TypeID < 2030043136
              -- ORDER BY achievement_tipo.ID_ACHIEVEMENT
         )  AS Z 
            LEFT OUTER JOIN 
            (
               SELECT 
                  X.IDX, 
                  X.ID_ACHIEVEMENT, 
                  X.TypeID_ACHIEVE, 
                  X.Data_Sec, 
                  Y.Count_ID, 
                  Y.TypeID AS COUNT_TYPEID, 
                  Y.Count_Num_Item
               FROM 
                  (
                     SELECT 
                        achievement_quest.IDX, 
                        achievement_quest.ID_ACHIEVEMENT, 
                        achievement_quest.TypeID_ACHIEVE, 
                        achievement_quest.Data_Sec, 
                        achievement_quest.Count_ID
                     FROM pangya.achievement_quest
                     WHERE achievement_quest.UID = _IDUSER
                  )  AS X 
                     INNER JOIN 
                     (
                        SELECT count_item.TypeID, count_item.Count_ID, count_item.Count_Num_Item
                        FROM pangya.count_item
                        WHERE count_item.UID = _IDUSER
                     )  AS Y 
                     ON X.Count_ID = Y.Count_ID
            )  AS A 
            ON Z.ID_ACHIEVEMENT = A.ID_ACHIEVEMENT
         ORDER BY A.IDX;
END;
$$;


ALTER FUNCTION pangya.procgetachievement_new(_iduser integer) OWNER TO postgres;

--
-- TOC entry 561 (class 1255 OID 23247)
-- Name: procgetachievementquest(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetachievementquest(_iduser integer) RETURNS TABLE("_ACHIEVEMENT_TYPEID_" integer, "_ACHIEVEMENT_ID_" bigint, "_ACHIEVEMENT_QUEST_TYPEID_" integer, "_ACHIEVEMENT_COUNTER_NUM_" bigint, "_ACHIEVEMENT_COUNTER_DATA_SEC_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT 
         Z.TypeID AS ACHIEVEMENT_TYPEID, 
         Z.ID_ACHIEVEMENT AS ACHIEVEMENT_ID, 
         A.TypeID_ACHIEVE AS ACHIEVEMENT_QUEST_TYPEID, 
         A.Count_Num_Item AS ACHIEVEMENT_COUNTER_NUM, 
         A.Data_Sec AS ACHIEVEMENT_COUNTER_DATA_SEC
      FROM 
         (
            SELECT achievement_tipo.TypeID, achievement_tipo.ID_ACHIEVEMENT
            FROM pangya.achievement_tipo
            WHERE achievement_tipo.UID = _IDUSER AND achievement_tipo.TypeID < 2030043136
               ORDER BY achievement_tipo.ID_ACHIEVEMENT LIMIT 9223372036854775807
         )  AS Z 
            LEFT OUTER JOIN 
            (
               SELECT 
                  X.IDX, 
                  X.ID_ACHIEVEMENT, 
                  X.TypeID_ACHIEVE, 
                  X.Data_Sec, 
                  Y.Count_Num_Item
               FROM 
                  (
                     SELECT 
                        achievement_quest.IDX, 
                        achievement_quest.ID_ACHIEVEMENT, 
                        achievement_quest.TypeID_ACHIEVE, 
                        achievement_quest.Data_Sec, 
                        achievement_quest.Count_ID
                     FROM pangya.achievement_quest
                     WHERE achievement_quest.UID = _IDUSER
                  )  AS X 
                     INNER JOIN 
                     (
                        SELECT count_item.Count_Num_Item, count_item.Count_ID
                        FROM pangya.count_item
                        WHERE count_item.UID = _IDUSER
                     )  AS Y 
                     ON X.Count_ID = Y.Count_ID
            )  AS A 
            ON Z.ID_ACHIEVEMENT = A.ID_ACHIEVEMENT
         ORDER BY A.IDX;
END;
$$;


ALTER FUNCTION pangya.procgetachievementquest(_iduser integer) OWNER TO postgres;

--
-- TOC entry 563 (class 1255 OID 23248)
-- Name: procgetachievequest(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetachievequest(_iduser integer) RETURNS TABLE("_TypeID_ACHIEVE_" integer, "_TypeID_" integer, "_Count_ID_" integer, "_Data_Sec_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT B.TypeID_ACHIEVE, COALESCE(C.TypeID, 0) AS TypeID, COALESCE(C.Count_ID, 0) AS Count_ID, COALESCE(C.Data_Sec, 0) AS Data_Sec
      FROM 
         (
            SELECT 
               a.TypeID, 
               D.IDX, 
               D.UID, 
               D.ID_ACHIEVEMENT, 
               D.TypeID_ACHIEVE, 
               D.Count_ID, 
               D.Data_Sec, 
               D.Objetivo_Quest
            FROM pangya.achievement_tipo  AS a, pangya.achievement_quest  AS D
            WHERE D.UID = _IDUSER AND a.ID_ACHIEVEMENT = D.ID_ACHIEVEMENT
         )  AS B 
            LEFT OUTER JOIN 
            (
               SELECT 
                  count_item.UID, 
                  count_item.Nome, 
                  count_item.ID_ACHIEVEMENT, 
                  count_item.TypeID, 
                  count_item.Count_ID, 
                  count_item.Count_Num_Item, 
                  count_item.Data_Sec, 
                  count_item.TIPO
               FROM pangya.count_item
               WHERE count_item.UID = _IDUSER
            )  AS C 
            ON C.Count_ID = B.Count_ID
      WHERE B.UID = _IDUSER
         ORDER BY B.IDX;
END;
$$;


ALTER FUNCTION pangya.procgetachievequest(_iduser integer) OWNER TO postgres;

--
-- TOC entry 564 (class 1255 OID 23249)
-- Name: procgetachievequestgui(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetachievequestgui(_iduser integer) RETURNS TABLE("_TypeID_ACHIEVE_" integer, "_Count_Num_Item_" bigint, "_Data_Sec_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT B.TypeID_ACHIEVE, min(c.Count_Num_Item) AS Count_Num_Item, min(c.Data_Sec) AS Data_Sec
      FROM pangya.achievement_tipo  AS A, pangya.achievement_quest  AS B, pangya.count_item  AS c
      WHERE 
         A.ID_ACHIEVEMENT = B.ID_ACHIEVEMENT AND 
         B.Count_ID = c.Count_ID AND 
         A.UID = _IDUSER AND 
         A.TypeID < 1946157056
      GROUP BY B.TypeID_ACHIEVE
         ORDER BY min(B.IDX);
END;
$$;


ALTER FUNCTION pangya.procgetachievequestgui(_iduser integer) OWNER TO postgres;

--
-- TOC entry 565 (class 1255 OID 23250)
-- Name: procgetachievetype(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetachievetype(_iduser integer) RETURNS TABLE("_TIPO_" smallint, "_TypeID_" integer, "_ID_ACHIEVEMENT_" bigint, "_Option_" integer, "_COUNTER_" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT 
         min(A.TIPO) AS TIPO, 
         min(A.TypeID) AS TypeID, 
         min(A.ID_ACHIEVEMENT) AS ID_ACHIEVEMENT, 
         min(A.Option) AS Option, 
         count(b.ID_ACHIEVEMENT) AS COUNTER
      FROM pangya.achievement_tipo  AS A, pangya.achievement_quest  AS b
      WHERE 
         A.ID_ACHIEVEMENT = b.ID_ACHIEVEMENT AND 
         A.UID = _IDUSER AND 
         A.TIPO <> 2
      GROUP BY b.ID_ACHIEVEMENT
         ORDER BY b.ID_ACHIEVEMENT;
END;
$$;


ALTER FUNCTION pangya.procgetachievetype(_iduser integer) OWNER TO postgres;

--
-- TOC entry 566 (class 1255 OID 23251)
-- Name: procgetachievetypegui(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetachievetypegui(_iduser integer) RETURNS TABLE("_TypeID_" integer, "_ID_ACHIEVEMENT_" bigint, "_COUNTER_" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT min(A.TypeID) AS TypeID, min(A.ID_ACHIEVEMENT) AS ID_ACHIEVEMENT, count(b.ID_ACHIEVEMENT) AS COUNTER
      FROM pangya.achievement_tipo  AS A, pangya.achievement_quest  AS b
      WHERE 
         A.ID_ACHIEVEMENT = b.ID_ACHIEVEMENT AND 
         A.UID = _IDUSER AND 
         A.TypeID < 1946157056
      GROUP BY b.ID_ACHIEVEMENT
         ORDER BY b.ID_ACHIEVEMENT;
END;
$$;


ALTER FUNCTION pangya.procgetachievetypegui(_iduser integer) OWNER TO postgres;

--
-- TOC entry 567 (class 1255 OID 23252)
-- Name: procgetalldonationandepinbyplayer(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetalldonationandepinbyplayer(_uid integer) RETURNS TABLE(_code_ character varying, _plataforma_ smallint, _date_ timestamp without time zone, _gross_amount_ double precision, _status_ smallint, _epin_ uuid, _cp_qntd_ bigint, _epin_valid_ smallint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _UID > 0 THEN
	BEGIN
		
		RETURN QUERY SELECT
			a.code,
			a.plataforma,
			a.date,
			a.gross_amount,
			a.status,
			b.epin,
			b.qntd as cp_qntd,
			b.valid as epin_valid
		FROM
			pangya.pangya_donation_new a
			LEFT OUTER JOIN
			pangya.pangya_donation_epin b
			ON a.index = b.donation_id AND a.epin_id = b.index
		WHERE
			a.UID = _UID;
	END;
	ELSE
		RETURN QUERY SELECT '-1'::varchar(50) AS CODE,
				0::smallint,
				null::timestamp,
				0::float,
				0::smallint,
				null::uuid,
				0::bigint,
				0::smallint;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetalldonationandepinbyplayer(_uid integer) OWNER TO postgres;

--
-- TOC entry 568 (class 1255 OID 23253)
-- Name: procgetalldonationsandepin(); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetalldonationsandepin() RETURNS TABLE(_code_ character varying, _date_ timestamp without time zone, _email_ character varying, _escrow_ timestamp without time zone, _gross_amount_ double precision, _net_amount_ double precision, _plataforma_ smallint, _epin_id_ bigint, _reference_ character varying, _reg_date_ timestamp without time zone, _status_ smallint, _type_ smallint, _update_ timestamp without time zone, _uid_ integer, _index_ bigint, _epin_ uuid, _cp_qntd_ bigint, _epin_get_uid_ integer, _epin_valid_ smallint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT
		a.code,
		a.date,
		a.email,
		a.escrow,
		a.gross_amount,
		a.net_amount,
		a.plataforma,
		a.epin_id,
		a.reference,
		a.reg_date,
		a.status,
		a.type,
		a.update,
		a.uid,
		a.index,
		b.epin,
		b.qntd as cp_qntd,
		b.retrive_uid as epin_get_uid,
		b.valid as epin_valid
	FROM
		pangya.pangya_donation_new a
		LEFT OUTER JOIN
		pangya.pangya_donation_epin b
		ON a.index = b.donation_id AND a.epin_id = b.index;
END;
$$;


ALTER FUNCTION pangya.procgetalldonationsandepin() OWNER TO postgres;

--
-- TOC entry 569 (class 1255 OID 23254)
-- Name: procgetallemailfrommailbox(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetallemailfrommailbox(_iduser integer) RETURNS TABLE("_Msg_ID_" bigint, _fromid_ character varying, _giftdate_ timestamp without time zone, _message_ character varying, "_Contador_Vista_" integer, "_Lida_YN_" smallint, "_ITEM_ID_" integer, "_ITEM_TYPEID_" integer, "_FLAG_" integer, "_QUANTIDADE_ITEM_" integer, "_QUANTIDADE_DIA_" integer, "_PANG_" bigint, "_COOKIE_" bigint, "_GM_ID_" integer, "_FLAG_GIFT_" integer, "_UCC_IMG_MARK_" character varying, "_TYPE_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDUSER > 0 THEN
		
        RETURN QUERY SELECT 
            A.Msg_ID, 
            A.fromid,  
			A.giftdate, 
            A.message, 
            A.Contador_Vista, 
            A.Lida_YN, 
            COALESCE(B.item_id, 0) AS ITEM_ID,
            COALESCE(B.item_typeid, 0) AS ITEM_TYPEID, 
            COALESCE(B.Flag, 0) AS FLAG, 
			COALESCE(B.Quantidade_item, 0) AS QUANTIDADE_ITEM, 
			COALESCE(B.Quantidade_Dia, 0) AS QUANTIDADE_DIA, 
            COALESCE(B.Pang, 0) AS PANG, 
			COALESCE(B.Cookie, 0) AS COOKIE, 
			COALESCE(B.GM_ID, 0) AS GM_ID, 
			COALESCE(B.Flag_Gift, 0) AS FLAG_GIFT,  
			COALESCE((SELECT UCCIDX FROM pangya.tu_ucc WHERE item_id = B.item_id), '0') AS UCC_IMG_MARK, 
            COALESCE(B.Type, 0) AS TYPE
        FROM 
        (
            SELECT 
				UID, 
				Msg_ID, 
				fromid, 
				message, 
				giftdate, 
				Flag, 
				Contador_Vista, 
				Lida_YN
            FROM pangya.pangya_gift_table
            WHERE valid = 1
        )  AS A 
        LEFT OUTER JOIN 
        (
            SELECT 
				Msg_ID AS ID_MSG, 
				item_id, 
				item_typeid, 
				Flag, 
				Quantidade_item, 
				Quantidade_Dia, 
				Pang, 
				Cookie, 
				GM_ID, 
				Flag_Gift, 
				UCC_IMG_MARK, 
				Type
            FROM pangya.pangya_item_mail
            WHERE valid = 1
        )  AS B 
        ON A.Msg_ID = B.ID_MSG
		WHERE UID = _IDUSER;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetallemailfrommailbox(_iduser integer) OWNER TO postgres;

--
-- TOC entry 570 (class 1255 OID 23255)
-- Name: procgetallitembyplayer(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetallitembyplayer(_iduser integer) RETURNS TABLE("_TYPEID_" integer, "_TYPE_" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDUSER > 0 THEN

		-- Select Caddie item
		RETURN QUERY SELECT typeid, 0::bigint as TYPE FROM pangya.pangya_caddie_information WHERE UID = _IDUSER AND Valid = 1 AND typeid > 0
		GROUP BY typeid 
		ORDER BY typeid;

		-- Select Mascot item
		RETURN QUERY SELECT typeid, 1::bigint as TYPE FROM pangya.pangya_mascot_info WHERE UID = _IDUSER AND Valid = 1 AND
			(Tipo != 1 OR Period = 0 OR EndDate = RegDate OR DATEDIFF('SECOND', now()::timestamp, EndDate) > 0)
			AND typeid > 0
		GROUP BY typeid 
		ORDER BY typeid;

		-- Select Warehouse item
		RETURN QUERY SELECT typeid, 2::bigint as TYPE FROM pangya.pangya_item_warehouse WHERE UID = _IDUSER AND valid = 1 AND 
			Gift_Flag = 0 AND (flag = 96 OR applytime = enddate OR typeid = x'1A000042'::int/*TICKET REPORT*/ OR enddate > CURRENT_TIMESTAMP)
			AND typeid > 0
		GROUP BY typeid
		ORDER BY typeid;

		-- Dolphin Locker
		RETURN QUERY SELECT
			a.typeid, 3::bigint as TYPE
		FROM 
			pangya.pangya_item_warehouse a
			INNER JOIN pangya.pangya_dolfini_locker_item c
			ON a.item_id = c.item_id
		WHERE a.uid = _IDUSER AND c.flag = 1 AND a.typeid > 0
		GROUP BY a.typeid
		ORDER BY a.typeid;

		-- Mail Box
		RETURN QUERY SELECT
			b.item_typeid as typeid, 4::bigint as TYPE
		FROM
			pangya.pangya_gift_table a
			INNER JOIN
			pangya.pangya_item_mail b
			ON a.Msg_ID = b.Msg_ID
		WHERE
			a.UID = _IDUSER AND a.valid = 1 AND b.valid = 1
			AND b.item_typeid > 0
		GROUP BY b.item_typeid
		ORDER BY b.item_typeid;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetallitembyplayer(_iduser integer) OWNER TO postgres;

--
-- TOC entry 571 (class 1255 OID 23256)
-- Name: procgetapproachmissions(); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetapproachmissions() RETURNS TABLE(_numero_ bigint, _tipo_ integer, _reward_tipo_ integer, _box_ integer, _flag_ integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT 
         numero, 
         tipo, 
         reward_tipo, 
         box, 
         flag
      FROM pangya.pangya_approach_missions
      WHERE active = 1;
END;
$$;


ALTER FUNCTION pangya.procgetapproachmissions() OWNER TO postgres;

--
-- TOC entry 572 (class 1255 OID 23257)
-- Name: procgetattendancereward(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetattendancereward(_iduser integer) RETURNS TABLE(_counter_ integer, _item_typeid_now_ integer, _item_qntd_now_ integer, _item_typeid_after_ integer, _item_qntd_after_ integer, _last_login_ timestamp without time zone)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT 
         counter, 
         item_typeid_now, 
         item_qntd_now, 
         item_typeid_after, 
         item_qntd_after,
		 last_login
      FROM pangya.pangya_attendance_reward
      WHERE UID = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.procgetattendancereward(_iduser integer) OWNER TO postgres;

--
-- TOC entry 573 (class 1255 OID 23258)
-- Name: procgetauthkeygame(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetauthkeygame(_iduser integer, _serverid integer) RETURNS TABLE("_AuthKey_" character varying, "_ServerID_" integer, _valid_ smallint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT AuthKey, ServerID, valid
	FROM pangya.authkey_game
	WHERE UID = _IDUSER AND ServerID = _SERVERID;

	UPDATE pangya.authkey_game SET valid = 0 WHERE UID = _IDUSER AND ServerID = _SERVERID;
END;
$$;


ALTER FUNCTION pangya.procgetauthkeygame(_iduser integer, _serverid integer) OWNER TO postgres;

--
-- TOC entry 574 (class 1255 OID 23259)
-- Name: procgetauthkeylogin(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetauthkeylogin(_iduser integer) RETURNS TABLE("_ATUH_KEY_" character varying, "_VALID_" smallint)
    LANGUAGE plpgsql
    AS $$
BEGIN

	RETURN QUERY SELECT AuthKey, valid
	FROM pangya.authkey_login
	WHERE UID = _IDUSER;
	
	UPDATE pangya.authkey_login SET valid = 0 WHERE UID = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.procgetauthkeylogin(_iduser integer) OWNER TO postgres;

--
-- TOC entry 575 (class 1255 OID 23260)
-- Name: procgetblackpapelitem(); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetblackpapelitem() RETURNS TABLE("_Typeid_" integer, "_Tipo_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT Typeid, Tipo
      FROM pangya.black_papel_item
         ORDER BY black_papel_item.Tipo;
END;
$$;


ALTER FUNCTION pangya.procgetblackpapelitem() OWNER TO postgres;

--
-- TOC entry 576 (class 1255 OID 23261)
-- Name: procgetblackpapelrate(); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetblackpapelrate() RETURNS TABLE("_Nome_" character varying, "_Probabilidade_" integer, _tipo_ integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT Nome, Probabilidade, tipo
      FROM pangya.black_papel_rate;
END;
$$;


ALTER FUNCTION pangya.procgetblackpapelrate() OWNER TO postgres;

--
-- TOC entry 577 (class 1255 OID 23262)
-- Name: procgetboxinfo(); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetboxinfo() RETURNS TABLE(_id_ integer, _typeid_ integer, _numero_ integer, _tipo_open_ smallint, _tipo_ smallint, _opened_typeid_ integer, _message_ character varying, _item_typeid_ integer, _item_numero_ integer, _item_probabilidade_ integer, _item_qntd_ integer, _item_raridade_ smallint, _item_duplicar_ smallint, _item_active_ smallint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT 
		 a.id
		,a.typeid
		,a.numero
		,a.tipo_open
		,a.tipo
		,a.opened_typeid
		,a.message
		,b.typeid as item_typeid
		,b.numero as item_numero
		,b.probabilidade as item_probabilidade
		,b.qntd as item_qntd
		,b.raridade as item_raridade
		,b.duplicar as item_duplicar
		,b.active as item_active
	FROM pangya.pangya_new_box as a
	INNER JOIN
	pangya.pangya_new_box_item as b
	ON a.id = b.box_id AND (a.numero = b.numero OR b.numero = -1)
	WHERE a.active = 1;
END;
$$;


ALTER FUNCTION pangya.procgetboxinfo() OWNER TO postgres;

--
-- TOC entry 578 (class 1255 OID 23263)
-- Name: procgetbpcouponsuser(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetbpcouponsuser(_iduser integer) RETURNS TABLE("_TYPEID_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_ITEMTYPEID int = 0;
	_QNTD int = 0;
	_C0_QNTD int = 0;
	_COUNT int = 0;
	_OPT int = 0;
BEGIN
	
	_ITEMTYPEID = 436207656;

	_COUNT = 0;
	
	<< loop_coupon >>
	WHILE (1 = 1) LOOP

		IF _COUNT < 3 THEN

			SELECT pangya_item_warehouse.item_id INTO _QNTD
			FROM pangya.pangya_item_warehouse
			WHERE pangya_item_warehouse.UID = _IDUSER AND pangya_item_warehouse.typeid = _ITEMTYPEID + _COUNT;

			SELECT pangya_item_warehouse.C0 INTO _C0_QNTD
			FROM pangya.pangya_item_warehouse
			WHERE pangya_item_warehouse.UID = _IDUSER AND pangya_item_warehouse.typeid = _ITEMTYPEID + _COUNT;

			IF _QNTD > 0 AND _C0_QNTD > 0 THEN

				_ITEMTYPEID = _ITEMTYPEID + _COUNT;

				RETURN QUERY SELECT pangya_item_warehouse.typeid
				FROM pangya.pangya_item_warehouse
				WHERE pangya_item_warehouse.UID = _IDUSER AND pangya_item_warehouse.typeid = _ITEMTYPEID;

				_OPT = 1;

				-- exit
				exit loop_coupon;

			END IF;

			_COUNT = _COUNT + 1;

			-- continue
			CONTINUE;
			
		END IF;

		-- exit
		exit loop_coupon;

	END LOOP loop_coupon;

	IF _OPT = 0 THEN

		UPDATE pangya.user_info
			SET 
				Pang = user_info.Pang - 900
		WHERE user_info.UID = _IDUSER;

		RETURN QUERY SELECT _OPT AS typeid;

	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetbpcouponsuser(_iduser integer) OWNER TO postgres;

--
-- TOC entry 562 (class 1255 OID 23264)
-- Name: procgetcaddieferias(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetcaddieferias(_iduser integer) RETURNS TABLE(_item_id_ bigint, "_UID_" integer, _typeid_ integer, _parts_typeid_ integer, "_cLevel_" smallint, "_Exp_" integer, "_RentFlag_" smallint, "_EndDate_" timestamp without time zone, "_Purchase_" smallint, "_parts_EndDate_" timestamp without time zone, "_CheckEnd_" smallint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	
	IF _IDUSER > 0 THEN
		RETURN QUERY SELECT 
			item_id, 
			UID, 
			typeid, 
			parts_typeid,
			cLevel, 
			Exp, 
			RentFlag, 
			EndDate,
			Purchase, 
			parts_EndDate,
			CheckEnd
		FROM pangya.pangya_caddie_information
		WHERE pangya_caddie_information.UID = _IDUSER 
			AND pangya_caddie_information.Valid = 1
			AND (datediff('second', now()::timestamp, pangya_caddie_information.EndDate) <= 0 
				AND pangya_caddie_information.RentFlag = 2
				AND pangya_caddie_information.CheckEnd = 1);
	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetcaddieferias(_iduser integer) OWNER TO postgres;

--
-- TOC entry 554 (class 1255 OID 23265)
-- Name: procgetcaddieinfo(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetcaddieinfo(_iduser integer) RETURNS TABLE(_item_id_ bigint, "_UID_" integer, _typeid_ integer, _parts_typeid_ integer, "_cLevel_" smallint, "_Exp_" integer, "_RentFlag_" smallint, "_EndDate_" timestamp without time zone, "_Purchase_" smallint, "_parts_EndDate_" timestamp without time zone, "_CheckEnd_" smallint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	
	IF _IDUSER > 0 THEN
		RETURN QUERY SELECT 
			item_id, 
			UID, 
			typeid, 
			parts_typeid,
			cLevel, 
			Exp, 
			RentFlag, 
			EndDate,
			Purchase, 
			parts_EndDate,
			CheckEnd
		FROM pangya.pangya_caddie_information
		WHERE pangya_caddie_information.UID = _IDUSER 
		AND pangya_caddie_information.Valid = 1;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetcaddieinfo(_iduser integer) OWNER TO postgres;

--
-- TOC entry 580 (class 1255 OID 23266)
-- Name: procgetcaddieinfo_one(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetcaddieinfo_one(_iduser integer, _caddieid integer) RETURNS TABLE(_item_id_ bigint, "_UID_" integer, _typeid_ integer, _parts_typeid_ integer, "_cLevel_" smallint, "_Exp_" integer, "_RentFlag_" smallint, "_EndDate_" timestamp without time zone, "_Purchase_" smallint, "_parts_EndDate_" timestamp without time zone, "_CheckEnd_" smallint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	
	IF _IDUSER > 0 AND _CaddieID > 0 THEN
	BEGIN
		RETURN QUERY SELECT 
			item_id, 
			UID, 
			typeid, 
			parts_typeid,
			cLevel, 
			Exp, 
			RentFlag, 
			EndDate,
			Purchase, 
			parts_EndDate,
			CheckEnd
		FROM pangya.pangya_caddie_information
		WHERE pangya_caddie_information.UID = _IDUSER 
			AND pangya_caddie_information.Valid = 1
			AND item_id = _CaddieID;
	END;
	ELSE
		RETURN QUERY SELECT -1::bigint AS ITEM_ID,
					0::int,
					0::int,
					0::int,
					0::smallint,
					0::int,
					0::smallint,
					null::timestamp,
					0::smallint,
					null::timestamp,
					0::smallint;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetcaddieinfo_one(_iduser integer, _caddieid integer) OWNER TO postgres;

--
-- TOC entry 581 (class 1255 OID 23267)
-- Name: procgetcardequip(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetcardequip(_iduser integer, _opt integer) RETURNS TABLE(_index_ bigint, _card_typeid_ integer, "_UID_" integer, _parts_typeid_ integer, _parts_id_ integer, "_Efeito_" integer, "_Efeito_Qntd_" integer, "_Slot_" integer, "_USE_DT_" character varying, "_END_DT_" character varying, "_TEMPO_" integer, "_Tipo_" integer, "_USE_YN_" smallint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	
	IF _OPT = 0 THEN
	BEGIN

		RETURN QUERY SELECT 
			index,
			card_typeid, 
			UID, 
			parts_typeid, 
			parts_id, 
			Efeito, 
			Efeito_Qntd, 
			Slot, 
			COALESCE(CAST(USE_DT AS VARCHAR), '') AS USE_DT, 
			COALESCE(CAST(END_DT AS VARCHAR), '') AS END_DT, 
			null::int AS TEMPO,
			Tipo, 
			USE_YN
		FROM pangya.pangya_card_equip
		WHERE 
			UID = _IDUSER AND 
			USE_YN = 1 AND 
			public.GroupID(card_typeid) BETWEEN 124 AND 125 AND (USE_DT IS NULL OR datediff('minute', now()::timestamp, END_DT) > 0)
			ORDER BY card_typeid;

	END;
	ELSIF _OPT = 1 THEN

		RETURN QUERY SELECT 
			-1::bigint as index,
			card_typeid, 
			UID, 
			parts_typeid, 
			parts_id, 
			Efeito, 
			Efeito_Qntd, 
			Slot, 
			COALESCE(CAST(USE_DT AS VARCHAR), '') AS USE_DT, 
			COALESCE(CAST(END_DT AS VARCHAR), '') AS END_DT,
			datediff('second', now()::timestamp, END_DT) AS TEMPO, 
			Tipo, 
			USE_YN
		FROM pangya.pangya_card_equip
		WHERE 
			UID = _IDUSER AND 
			USE_YN = 1 AND 
			public.GroupID(card_typeid) = 26 AND (USE_DT IS NULL OR datediff('minute', now()::timestamp, END_DT) > 0)
			ORDER BY card_typeid;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetcardequip(_iduser integer, _opt integer) OWNER TO postgres;

--
-- TOC entry 582 (class 1255 OID 23268)
-- Name: procgetcardinfo(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetcardinfo(_iduser integer) RETURNS TABLE(_card_itemid_ bigint, "_UID_" integer, _card_typeid_ integer, "_Slot_" integer, "_Efeito_" integer, "_Efeito_Qntd_" integer, "_QNTD_" integer, "_USE_DT_" timestamp without time zone, "_END_DT_" timestamp without time zone, _card_type_ smallint, "_USE_YN_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	
	RETURN QUERY SELECT 
         card_itemid, 
         UID, 
         card_typeid, 
         Slot, 
         Efeito, 
         Efeito_Qntd, 
         QNTD, 
          (CASE 
            WHEN CAST(REPLACE((REPLACE((REPLACE((CAST(USE_DT AS VARCHAR(20))), '-', '')), ' ', '')), ':', '') AS bigint) <> 0 THEN pangya_card.USE_DT
            ELSE NULL
         END) AS USE_DT, 
          (CASE 
            WHEN CAST(REPLACE((REPLACE((REPLACE((CAST(END_DT AS VARCHAR(20))), '-', '')), ' ', '')), ':', '') AS bigint) <> 0 THEN pangya_card.END_DT
            ELSE NULL
         END) AS END_DT, 
         card_type, 
          (CASE 
            WHEN USE_YN = 'N' THEN 0
            ELSE 1
         END) AS USE_YN
      FROM pangya.pangya_card
      WHERE UID = _IDUSER AND QNTD > 0
         ORDER BY card_itemid;
END;
$$;


ALTER FUNCTION pangya.procgetcardinfo(_iduser integer) OWNER TO postgres;

--
-- TOC entry 583 (class 1255 OID 23269)
-- Name: procgetcardinfo_one(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetcardinfo_one(_iduser integer, _iditem integer) RETURNS TABLE(_card_itemid_ bigint, "_UID_" integer, _card_typeid_ integer, "_Slot_" integer, "_Efeito_" integer, "_Efeito_Qntd_" integer, "_QNTD_" integer, "_USE_DT_" timestamp without time zone, "_END_DT_" timestamp without time zone, _card_type_ smallint, "_USE_YN_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	
	RETURN QUERY SELECT 
         card_itemid, 
         UID, 
         card_typeid, 
         Slot, 
         Efeito, 
         Efeito_Qntd, 
         QNTD, 
          (CASE 
            WHEN CAST(REPLACE((REPLACE((REPLACE((CAST(USE_DT AS VARCHAR(20))), '-', '')), ' ', '')), ':', '') AS bigint) <> 0 THEN pangya_card.USE_DT
            ELSE NULL
         END) AS USE_DT, 
          (CASE 
            WHEN CAST(REPLACE((REPLACE((REPLACE((CAST(END_DT AS VARCHAR(20))), '-', '')), ' ', '')), ':', '') AS bigint) <> 0 THEN pangya_card.END_DT
            ELSE NULL
         END) AS END_DT, 
         card_type, 
          (CASE 
            WHEN USE_YN = 'N' THEN 0
            ELSE 1
         END) AS USE_YN
      FROM pangya.pangya_card
      WHERE pangya_card.UID = _IDUSER AND pangya_card.card_itemid = _IDITEM;
END;
$$;


ALTER FUNCTION pangya.procgetcardinfo_one(_iduser integer, _iditem integer) OWNER TO postgres;

--
-- TOC entry 584 (class 1255 OID 23270)
-- Name: procgetcardpack(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetcardpack(_typeidcardpack integer) RETURNS TABLE("_TypeID_Card_" integer, "_Tipo_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	
	RETURN QUERY SELECT pangya_cards.TypeID_Card, pangya_cards.Tipo
      FROM pangya.pangya_cards
      WHERE pangya_cards.TypeID_Pack = _TYPEIDCARDPACK
         ORDER BY pangya_cards.Tipo;
END;
$$;


ALTER FUNCTION pangya.procgetcardpack(_typeidcardpack integer) OWNER TO postgres;

--
-- TOC entry 585 (class 1255 OID 23271)
-- Name: procgetcardrate(); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetcardrate() RETURNS TABLE(_nome_ character varying, _tipo_ integer, _probabilidade_ integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	
	RETURN QUERY SELECT pangya_card_rate.nome, pangya_card_rate.tipo, pangya_card_rate.probabilidade
      FROM pangya.pangya_card_rate;
END;
$$;


ALTER FUNCTION pangya.procgetcardrate() OWNER TO postgres;

--
-- TOC entry 587 (class 1255 OID 23272)
-- Name: procgetcharacterinfo(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetcharacterinfo(_iduser integer) RETURNS TABLE(_item_id_ bigint, "_UID_" integer, _typeid_ integer, _parts_1_ integer, _parts_2_ integer, _parts_3_ integer, _parts_4_ integer, _parts_5_ integer, _parts_6_ integer, _parts_7_ integer, _parts_8_ integer, _parts_9_ integer, _parts_10_ integer, _parts_11_ integer, _parts_12_ integer, _parts_13_ integer, _parts_14_ integer, _parts_15_ integer, _parts_16_ integer, _parts_17_ integer, _parts_18_ integer, _parts_19_ integer, _parts_20_ integer, _parts_21_ integer, _parts_22_ integer, _parts_23_ integer, _parts_24_ integer, _default_hair_ smallint, _default_shirts_ smallint, _gift_flag_ smallint, "_PCL0_" smallint, "_PCL1_" smallint, "_PCL2_" smallint, "_PCL3_" smallint, "_PCL4_" smallint, "_Purchase_" smallint, _auxparts_1_ integer, _auxparts_2_ integer, _auxparts_3_ integer, _auxparts_4_ integer, _auxparts_5_ integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	
	RETURN QUERY SELECT 
         item_id, 
         UID, 
         typeid, 
         parts_1, 
         parts_2, 
         parts_3, 
         parts_4, 
         parts_5, 
         parts_6, 
         parts_7, 
         parts_8, 
         parts_9, 
         parts_10, 
         parts_11, 
         parts_12, 
         parts_13, 
         parts_14, 
         parts_15, 
         parts_16, 
         parts_17, 
         parts_18, 
         parts_19, 
         parts_20, 
         parts_21, 
         parts_22, 
         parts_23, 
         parts_24, 
         default_hair, 
         default_shirts, 
         gift_flag, 
         PCL0, 
         PCL1, 
         PCL2, 
         PCL3, 
         PCL4, 
         Purchase, 
         auxparts_1, 
         auxparts_2, 
         auxparts_3, 
         auxparts_4, 
         auxparts_5
      FROM pangya.pangya_character_information
      WHERE UID = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.procgetcharacterinfo(_iduser integer) OWNER TO postgres;

--
-- TOC entry 588 (class 1255 OID 23273)
-- Name: procgetcharacterinfo_one(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetcharacterinfo_one(_iduser integer) RETURNS TABLE(_item_id_ bigint, "_UID_" integer, _typeid_ integer, _parts_1_ integer, _parts_2_ integer, _parts_3_ integer, _parts_4_ integer, _parts_5_ integer, _parts_6_ integer, _parts_7_ integer, _parts_8_ integer, _parts_9_ integer, _parts_10_ integer, _parts_11_ integer, _parts_12_ integer, _parts_13_ integer, _parts_14_ integer, _parts_15_ integer, _parts_16_ integer, _parts_17_ integer, _parts_18_ integer, _parts_19_ integer, _parts_20_ integer, _parts_21_ integer, _parts_22_ integer, _parts_23_ integer, _parts_24_ integer, _default_hair_ smallint, _default_shirts_ smallint, _gift_flag_ smallint, "_PCL0_" smallint, "_PCL1_" smallint, "_PCL2_" smallint, "_PCL3_" smallint, "_PCL4_" smallint, "_Purchase_" smallint, _auxparts_1_ integer, _auxparts_2_ integer, _auxparts_3_ integer, _auxparts_4_ integer, _auxparts_5_ integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_IDITEM int;
BEGIN

	SELECT pangya_user_equip.character_id INTO _IDITEM
	FROM pangya.pangya_user_equip
	WHERE pangya_user_equip.UID = _IDUSER;
	
	RETURN QUERY SELECT 
         item_id, 
         UID, 
         typeid, 
         parts_1, 
         parts_2, 
         parts_3, 
         parts_4, 
         parts_5, 
         parts_6, 
         parts_7, 
         parts_8, 
         parts_9, 
         parts_10, 
         parts_11, 
         parts_12, 
         parts_13, 
         parts_14, 
         parts_15, 
         parts_16, 
         parts_17, 
         parts_18, 
         parts_19, 
         parts_20, 
         parts_21, 
         parts_22, 
         parts_23, 
         parts_24, 
         default_hair, 
         default_shirts, 
         gift_flag, 
         PCL0, 
         PCL1, 
         PCL2, 
         PCL3, 
         PCL4, 
         Purchase, 
         auxparts_1, 
         auxparts_2, 
         auxparts_3, 
         auxparts_4, 
         auxparts_5
      FROM pangya.pangya_character_information
      WHERE UID = _IDUSER AND item_id = _IDITEM;
END;
$$;


ALTER FUNCTION pangya.procgetcharacterinfo_one(_iduser integer) OWNER TO postgres;

--
-- TOC entry 589 (class 1255 OID 23274)
-- Name: procgetcometrefillinfo(); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetcometrefillinfo() RETURNS TABLE(_typeid_ integer, _min_ smallint, _max_ smallint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT typeid, min, max FROM pangya.pangya_comet_refill;
END;
$$;


ALTER FUNCTION pangya.procgetcometrefillinfo() OWNER TO postgres;

--
-- TOC entry 590 (class 1255 OID 23275)
-- Name: procgetcommands(); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetcommands() RETURNS TABLE(_idx bigint, _command_id integer, _arg1 integer, _arg2 integer, _arg3 integer, _arg4 integer, _arg5 integer, _target integer, _flag smallint, _valid smallint, "_reserveDate" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT idx, command_id, arg1, arg2, arg3, arg4, arg5, target, flag, valid,
			COALESCE(public."UNIX_TIMESTAMP"(reserveDate), 0) as reserveDate
	FROM pangya.pangya_command 
	WHERE valid = 1 AND (reserveDate IS NULL OR now() >= reserveDate)
	ORDER BY regDate;
END;
$$;


ALTER FUNCTION pangya.procgetcommands() OWNER TO postgres;

--
-- TOC entry 591 (class 1255 OID 23276)
-- Name: procgetcountitem(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetcountitem(_iduser integer) RETURNS TABLE("_TIPO_" smallint, "_TypeID_" integer, "_Count_ID_" integer, "_Count_Num_Item_" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN

	RETURN QUERY SELECT count_item.TIPO, count_item.TypeID, count_item.Count_ID, count_item.Count_Num_Item
	FROM pangya.count_item
	WHERE count_item.UID = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.procgetcountitem(_iduser integer) OWNER TO postgres;

--
-- TOC entry 592 (class 1255 OID 23277)
-- Name: procgetcoursereward(); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetcoursereward() RETURNS TABLE("_COURSE_" smallint, "_PANGREWARD_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT COURSE, PANGREWARD
      FROM pangya.pangya_course_reward_treasure
         ORDER BY COURSE;
END;
$$;


ALTER FUNCTION pangya.procgetcoursereward() OWNER TO postgres;

--
-- TOC entry 593 (class 1255 OID 23278)
-- Name: procgetcourserewardone(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetcourserewardone(_course_in integer) RETURNS TABLE("_COURSE_" smallint, "_PANGREWARD_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN

	RETURN QUERY SELECT pangya_course_reward_treasure.COURSE, pangya_course_reward_treasure.PANGREWARD
      FROM pangya.pangya_course_reward_treasure
      WHERE pangya_course_reward_treasure.COURSE = _course_in;
END;
$$;


ALTER FUNCTION pangya.procgetcourserewardone(_course_in integer) OWNER TO postgres;

--
-- TOC entry 594 (class 1255 OID 23279)
-- Name: procgetcubecoinlocation(integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetcubecoinlocation(_tipo_in integer, _course_in integer, _hole_in integer, _config2_in integer) RETURNS TABLE(_tipo_ integer, _config2_ integer, "_X_" real, "_Y_" real, "_Z_" real)
    LANGUAGE plpgsql
    AS $$
BEGIN

	RETURN QUERY SELECT 
         pangya_cube_coin_location.tipo, 
         pangya_cube_coin_location.config2, 
         pangya_cube_coin_location.x AS X, 
         pangya_cube_coin_location.y AS Y, 
         pangya_cube_coin_location.z AS Z
      FROM pangya.pangya_cube_coin_location
      WHERE 
         pangya_cube_coin_location.tipo = _tipo_in AND 
         pangya_cube_coin_location.course = _course_in AND 
         pangya_cube_coin_location.hole = _hole_in AND 
         pangya_cube_coin_location.config2 = _config2_in;
END;
$$;


ALTER FUNCTION pangya.procgetcubecoinlocation(_tipo_in integer, _course_in integer, _hole_in integer, _config2_in integer) OWNER TO postgres;

--
-- TOC entry 586 (class 1255 OID 23280)
-- Name: procgetdailyachievequest(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetdailyachievequest(_iduser integer) RETURNS TABLE("_TypeID_ACHIEVE_" integer, "_TypeID_" integer, "_Count_ID_" integer, "_Data_Sec_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN

	RETURN QUERY SELECT B.TypeID_ACHIEVE, COALESCE(min(C.TypeID), 0) AS TypeID, COALESCE(min(C.Count_ID), 0) AS Count_ID, COALESCE(min(C.Data_Sec), 0) AS Data_Sec
      FROM 
         (
            SELECT 
               a.TypeID, 
               a.Option, 
               D.IDX, 
               D.UID, 
               D.ID_ACHIEVEMENT, 
               D.TypeID_ACHIEVE, 
               D.Count_ID, 
               D.Data_Sec, 
               D.Objetivo_Quest
            FROM pangya.achievement_tipo  AS a, pangya.achievement_quest  AS D
            WHERE D.UID = _IDUSER AND a.ID_ACHIEVEMENT = D.ID_ACHIEVEMENT
         )  AS B 
            LEFT OUTER JOIN 
            (
               SELECT 
                  count_item.UID, 
                  count_item.Nome, 
                  count_item.ID_ACHIEVEMENT, 
                  count_item.TypeID, 
                  count_item.Count_ID, 
                  count_item.Count_Num_Item, 
                  count_item.Data_Sec, 
                  count_item.TIPO
               FROM pangya.count_item
               WHERE count_item.UID = _IDUSER
            )  AS C 
            ON C.Count_ID = B.Count_ID
      WHERE 
         B.UID = _IDUSER AND 
         B.Option = 3 AND 
         B.TypeID >= 2013265920 AND 
         B.TypeID < 2017460224
      GROUP BY B.TypeID_ACHIEVE
         ORDER BY min(B.IDX);
END;
$$;


ALTER FUNCTION pangya.procgetdailyachievequest(_iduser integer) OWNER TO postgres;

--
-- TOC entry 596 (class 1255 OID 23281)
-- Name: procgetdailyachievetype(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetdailyachievetype(_iduser integer) RETURNS TABLE("_TIPO_" smallint, "_TypeID_" integer, "_ID_ACHIEVEMENT_" bigint, "_Option_" integer, "_COUNTER_" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN

	RETURN QUERY SELECT 
         min(A.TIPO) AS TIPO, 
         min(A.TypeID) AS TypeID, 
         A.ID_ACHIEVEMENT, 
         min(A.Option) AS Option, 
         count(B.ID_ACHIEVEMENT) AS COUNTER
      FROM 
         (
            SELECT achievement_tipo.TIPO, achievement_tipo.TypeID, achievement_tipo.ID_ACHIEVEMENT, achievement_tipo.Option
            FROM pangya.achievement_tipo
            WHERE 
               achievement_tipo.UID = _IDUSER AND 
               achievement_tipo.Option = 3 AND 
               achievement_tipo.TypeID >= 2013265920 AND 
               achievement_tipo.TypeID < 2021654529
         )  AS A 
            INNER JOIN 
            (
               SELECT achievement_quest.ID_ACHIEVEMENT
               FROM pangya.achievement_quest
            )  AS B 
            ON A.ID_ACHIEVEMENT = B.ID_ACHIEVEMENT
      GROUP BY A.ID_ACHIEVEMENT
         ORDER BY A.ID_ACHIEVEMENT;
END;
$$;


ALTER FUNCTION pangya.procgetdailyachievetype(_iduser integer) OWNER TO postgres;

--
-- TOC entry 597 (class 1255 OID 23282)
-- Name: procgetdailyquest_new(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetdailyquest_new(_iduser integer) RETURNS TABLE("_Reg_Date_" bigint, "_Reg_Date_player_" bigint, _today_quest_ bigint, _achieve_quest_1_ integer, _achieve_quest_2_ integer, _achieve_quest_3_ integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	--Get Daily Quest
	_today timestamp;
	_last_accept timestamp;
BEGIN

	_last_accept = (SELECT last_quest_accept FROM pangya.pangya_daily_quest_player WHERE uid = _IDUSER);
	_today = (SELECT today_quest FROM pangya.pangya_daily_quest_player WHERE uid = _IDUSER);
    
	RETURN QUERY SELECT CAST(public."UNIX_TIMESTAMP"(Reg_Date) AS bigint) AS Reg_Date,	-- Deprecated Column
			CASE WHEN _last_accept IS NULL THEN 0 ELSE CAST(public."UNIX_TIMESTAMP"(_last_accept) AS bigint) END AS Reg_Date_player, 
			CASE WHEN _today IS NULL THEN 0 ELSE CAST(public."UNIX_TIMESTAMP"(_today) AS bigint) END AS today_quest,
			achieve_quest_1, achieve_quest_2, achieve_quest_3
	FROM pangya.pangya_daily_quest;
END;
$$;


ALTER FUNCTION pangya.procgetdailyquest_new(_iduser integer) OWNER TO postgres;

--
-- TOC entry 598 (class 1255 OID 23283)
-- Name: procgetdolfinilockerinfo(bigint); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetdolfinilockerinfo(_uid bigint) RETURNS TABLE(_uid_ integer, _senha_ character varying, _pang_ bigint, _locker_ smallint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT uid, senha, pang, locker FROM pangya.pangya_dolfini_locker WHERE uid = _uid;
END;
$$;


ALTER FUNCTION pangya.procgetdolfinilockerinfo(_uid bigint) OWNER TO postgres;

--
-- TOC entry 599 (class 1255 OID 23284)
-- Name: procgetdolfinilockeritem(bigint); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetdolfinilockeritem(_uid bigint) RETURNS TABLE(_item_id_ bigint, _uid_ integer, _typeid_ integer, "_SD_NAME_" character varying, "_SD_IDX_" character varying, "_SD_SEQ_" integer, "_SD_COPIER_NICK_" character varying, "_SD_STATUS_" integer, "_IDX_DOLFINI_LOCKER_ITEM_" bigint, "_FLAG_DOLFINI_LOCKER_ITEM_" smallint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT
		a.item_id,
		a.uid,
		a.typeid,
		CASE WHEN b.UCC_NAME IS NULL THEN '' ELSE b.UCC_NAME END as SD_NAME,
		CASE WHEN b.UCCIDX IS NULL THEN '' ELSE b.UCCIDX END as SD_IDX,
		CASE WHEN b.SEQ IS NULL THEN 0 ELSE b.SEQ END as SD_SEQ,
		CASE WHEN b.COPIER_NICK IS NULL THEN '' ELSE b.COPIER_NICK END as SD_COPIER_NICK,
		--CASE WHEN b.COPIER IS NULL THEN 0 ELSE b.COPIER END as SD_COPIER,
		--CASE WHEN b.TRADE IS NULL THEN 0 ELSE b.TRADE END as SD_TRADE,
		--CASE WHEN b.Flag IS NULL THEN 0 ELSE b.Flag END as SD_FLAG,
		CASE WHEN b.STATUS IS NULL THEN 0 ELSE b.STATUS END as SD_STATUS,
		c.idx as IDX_DOLFINI_LOCKER_ITEM,
		c.flag as FLAG_DOLFINI_LOCKER_ITEM
	FROM 
		pangya.pangya_item_warehouse a
		LEFT OUTER JOIN
		pangya.tu_ucc b
	ON	a.item_id = b.ITEM_ID
	INNER JOIN pangya.pangya_dolfini_locker_item c
	ON a.item_id = c.item_id
	WHERE a.uid = _uid AND c.flag = 1;
END;
$$;


ALTER FUNCTION pangya.procgetdolfinilockeritem(_uid bigint) OWNER TO postgres;

--
-- TOC entry 600 (class 1255 OID 23285)
-- Name: procgetdolfinilockeritemone(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetdolfinilockeritemone(_iduser integer, _item_id integer) RETURNS TABLE(_item_id_ bigint, _uid_ integer, _typeid_ integer, "_SD_NAME_" character varying, "_SD_IDX_" character varying, "_SD_SEQ_" integer, "_SD_COPIER_NICK_" character varying, "_SD_STATUS_" integer, "_IDX_DOLFINI_LOCKER_ITEM_" bigint, "_FLAG_DOLFINI_LOCKER_ITEM_" smallint)
    LANGUAGE plpgsql
    AS $$
BEGIN

	IF _IDUSER > 0 AND _ITEM_ID > 0 THEN
		RETURN QUERY SELECT
			a.item_id,
			a.uid,
			a.typeid,
			CASE WHEN b.UCC_NAME IS NULL THEN '' ELSE b.UCC_NAME END as SD_NAME,
			CASE WHEN b.UCCIDX IS NULL THEN '' ELSE b.UCCIDX END as SD_IDX,
			CASE WHEN b.SEQ IS NULL THEN 0 ELSE b.SEQ END as SD_SEQ,
			CASE WHEN b.COPIER_NICK IS NULL THEN '' ELSE b.COPIER_NICK END as SD_COPIER_NICK,
			CASE WHEN b.STATUS IS NULL THEN 0 ELSE b.STATUS END as SD_STATUS,
			c.idx as IDX_DOLFINI_LOCKER_ITEM,
			c.flag as FLAG_DOLFINI_LOCKER_ITEM
		FROM 
			pangya.pangya_item_warehouse a
			LEFT OUTER JOIN
			pangya.tu_ucc b
		ON	a.item_id = b.ITEM_ID
		INNER JOIN pangya.pangya_dolfini_locker_item c
		ON a.item_id = c.item_id
		WHERE a.uid = _IDUSER AND a.item_id = _ITEM_ID AND c.flag = 1;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetdolfinilockeritemone(_iduser integer, _item_id integer) OWNER TO postgres;

--
-- TOC entry 601 (class 1255 OID 23286)
-- Name: procgetdonationinfo(); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetdonationinfo() RETURNS TABLE("_ADM_uid_" integer, _uid_ integer, _plataforma_ smallint, _cash_ integer, _cookie_point_ integer, _count_item_ bigint, _email_ character varying, _obs_ character varying, _red_date_ character varying)
    LANGUAGE plpgsql
    AS $$
BEGIN

	RETURN QUERY SELECT	a.ADM_uid,
		a.uid,
		a.plataforma,
		a.cash,
		a.cookie_point,
		(SELECT COUNT(donation_id) FROM pangya.pangya_donation_item_log WHERE donation_id = a.index) as count_item,
		a.email,
		a.obs,
		CAST(a.red_date AS VARCHAR(19)) AS red_date
	FROM pangya.pangya_donation_log a
	ORDER BY a.index;
END;
$$;


ALTER FUNCTION pangya.procgetdonationinfo() OWNER TO postgres;

--
-- TOC entry 602 (class 1255 OID 23287)
-- Name: procgetdropcourseconfig(); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetdropcourseconfig() RETURNS TABLE(_rate_mana_artefact_ integer, _rate_grand_prix_ticket_ integer, "_rate_SSC_ticket_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT rate_mana_artefact, rate_grand_prix_ticket, rate_SSC_ticket FROM pangya.pangya_new_course_drop LIMIT 1;
END;
$$;


ALTER FUNCTION pangya.procgetdropcourseconfig() OWNER TO postgres;

--
-- TOC entry 605 (class 1255 OID 23288)
-- Name: procgetemailnaolidafrommailbox(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetemailnaolidafrommailbox(_iduser integer) RETURNS TABLE("_Msg_ID_" bigint, _fromid_ character varying, _message_ character varying, _giftdate_ timestamp without time zone, "_Contador_Vista_" integer, "_Lida_YN_" smallint, "_CONTADOR_" bigint, "_ITEM_ID_" integer, "_ITEM_TYPEID_" integer, "_FLAG_" integer, "_QUANTIDADE_ITEM_" integer, "_QUANTIDADE_DIA_" integer, "_PANG_" bigint, "_COOKIE_" bigint, "_GM_ID_" integer, "_FLAG_GIFT_" integer, "_UCC_IMG_MARK_" character varying, "_TYPE_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT
         X.Msg_ID, 
         X.fromid, 
         X.message, 
         X.giftdate, 
         X.Contador_Vista, 
         X.Lida_YN, 
         (CASE 
            WHEN X.CONTADOR IS NULL THEN 0
            ELSE X.CONTADOR
         END) AS CONTADOR, 
         (CASE 
            WHEN X.item_id IS NULL THEN 0
            ELSE X.item_id
         END) AS ITEM_ID, 
         (CASE 
            WHEN X.item_typeid IS NULL THEN 0
            ELSE X.item_typeid
         END) AS ITEM_TYPEID, 
         (CASE 
            WHEN X.Flag IS NULL THEN 0
            ELSE X.Flag
         END) AS FLAG, 
         (CASE 
            WHEN X.Quantidade_item IS NULL THEN 0
            ELSE X.Quantidade_item
         END) AS QUANTIDADE_ITEM, 
         (CASE 
            WHEN X.Quantidade_Dia IS NULL THEN 0
            ELSE X.Quantidade_Dia
         END) AS QUANTIDADE_DIA, 
         (CASE 
            WHEN X.Pang IS NULL THEN 0
            ELSE X.Pang
         END) AS PANG, 
         (CASE 
            WHEN X.Cookie IS NULL THEN 0
            ELSE X.Cookie
         END) AS COOKIE, 
         (CASE 
            WHEN X.GM_ID IS NULL THEN 0
            ELSE X.GM_ID
         END) AS GM_ID, 
         (CASE 
            WHEN X.Flag_Gift IS NULL THEN 0
            ELSE X.Flag_Gift
         END) AS FLAG_GIFT, 
         (CASE 
            WHEN X.UCC_IMG_MARK IS NULL THEN '0'
            ELSE X.UCC_IMG_MARK
         END) AS UCC_IMG_MARK, 
         (CASE 
            WHEN X.Type IS NULL THEN 0
            ELSE X.Type
         END) AS TYPE
      FROM 
         (
            SELECT
               C.UID, 
               C.Msg_ID, 
               C.fromid, 
               C.message, 
               C.giftdate, 
               C.Contador_Vista, 
               C.Lida_YN, 
               C.Flag AS Flag_MSG, 
               (SELECT count(Msg_ID) FROM pangya.pangya_item_mail WHERE Msg_ID = C.Msg_ID AND valid = 1 GROUP BY Msg_ID) AS CONTADOR, 
               C.IDMSG, 
			   C.item_id, 
               C.item_typeid, 
               C.Flag, 
               C.Quantidade_item,
               C.Quantidade_Dia, 
               C.GET_DATE, 
               C.Pang, 
               C.Cookie, 
               C.GM_ID, 
               C.Flag_Gift, 
               /*B.UCC_IMG_MARK*/
			   COALESCE((SELECT UCCIDX FROM pangya.tu_ucc WHERE item_id = C.item_id), '0') AS UCC_IMG_MARK, 
               C.Type
            FROM 
               (
				SELECT FIRST_VALUE(B.item_id) OVER(PARTITION BY B.IDMSG ORDER BY B.IDMSG) AS item_id, 
						FIRST_VALUE(B.item_typeid) OVER(PARTITION BY B.IDMSG ORDER BY B.IDMSG) AS item_typeid, 
						FIRST_VALUE(B.Flag) OVER(PARTITION BY B.IDMSG ORDER BY B.IDMSG) AS Flag, 
						FIRST_VALUE(B.Quantidade_item) OVER(PARTITION BY B.IDMSG ORDER BY B.IDMSG) AS Quantidade_item, 
						FIRST_VALUE(B.Quantidade_Dia) OVER(PARTITION BY B.IDMSG ORDER BY B.IDMSG) AS Quantidade_Dia, 
						FIRST_VALUE(B.GET_DATE) OVER(PARTITION BY B.IDMSG ORDER BY B.IDMSG) AS GET_DATE, 
						FIRST_VALUE(B.Pang) OVER(PARTITION BY B.IDMSG ORDER BY B.IDMSG) AS Pang, 
						FIRST_VALUE(B.Cookie) OVER(PARTITION BY B.IDMSG ORDER BY B.IDMSG) AS Cookie, 
						FIRST_VALUE(B.GM_ID) OVER(PARTITION BY B.IDMSG ORDER BY B.IDMSG) AS GM_ID, 
						FIRST_VALUE(B.Flag_Gift) OVER(PARTITION BY B.IDMSG ORDER BY B.IDMSG) AS Flag_Gift, 
						FIRST_VALUE(B.UCC_IMG_MARK) OVER(PARTITION BY B.IDMSG ORDER BY B.IDMSG) AS UCC_IMG_MARK, 
						FIRST_VALUE(B.Type) OVER(PARTITION BY B.IDMSG ORDER BY B.IDMSG) AS Type,
						A.UID, 
						A.Msg_ID, 
						A.fromid, 
						A.message, 
						A.giftdate, 
						A.Contador_Vista, 
						A.Lida_YN, 
						A.Flag AS Flag_MSG,
						B.IDMSG
				FROM(
						SELECT 
							pangya_gift_table.UID, 
							pangya_gift_table.Msg_ID, 
							pangya_gift_table.fromid, 
							pangya_gift_table.message, 
							pangya_gift_table.giftdate, 
							pangya_gift_table.Flag, 
							pangya_gift_table.Contador_Vista, 
							pangya_gift_table.Lida_YN
						FROM pangya.pangya_gift_table
						WHERE pangya_gift_table.valid = 1 AND pangya_gift_table.Lida_YN = 0/*Não lida*/
					)  AS A 
						LEFT OUTER JOIN 
						(
							SELECT
							pangya_item_mail.Msg_ID AS IDMSG, 
							pangya_item_mail.item_id, 
							pangya_item_mail.item_typeid, 
							pangya_item_mail.Flag, 
							pangya_item_mail.Quantidade_item, 
							pangya_item_mail.Quantidade_Dia, 
							pangya_item_mail.GET_DATE, 
							pangya_item_mail.Pang, 
							pangya_item_mail.Cookie, 
							pangya_item_mail.GM_ID, 
							pangya_item_mail.Flag_Gift, 
							pangya_item_mail.UCC_IMG_MARK, 
							pangya_item_mail.Type
							FROM pangya.pangya_item_mail
							WHERE pangya_item_mail.valid = 1
						)  AS B 
						ON A.Msg_ID = B.IDMSG
						--WHERE B.item_id IS NOT NULL
				  ) AS C
				 -- WHERE C.item_id IS NOT NULL
				  GROUP BY C.UID, C.Msg_ID, C.fromid, C.message, C.giftdate, C.Contador_Vista, C.Lida_YN, C.Flag, C.IDMSG, C.item_id, C.item_typeid,
							C.Quantidade_item, C.Quantidade_Dia, C.GET_DATE, C.Pang, C.Cookie, C.GM_ID, C.Flag_Gift, C.UCC_IMG_MARK, C.Type
         )  AS X
      WHERE X.UID = _IDUSER
         ORDER BY X.Msg_ID DESC
		 LIMIT 300;
END;
$$;


ALTER FUNCTION pangya.procgetemailnaolidafrommailbox(_iduser integer) OWNER TO postgres;

--
-- TOC entry 606 (class 1255 OID 23289)
-- Name: procgetemailnaolidafrommailboxone(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetemailnaolidafrommailboxone(_iduser integer, _msg_id_in integer) RETURNS TABLE("_Msg_ID_" bigint, _fromid_ character varying, _message_ character varying, _giftdate_ timestamp without time zone, "_Contador_Vista_" integer, "_Lida_YN_" smallint, "_CONTADOR_" bigint, _item_id_ integer, _item_typeid_ integer, "_Flag_" integer, "_Quantidade_item_" integer, "_Quantidade_Dia_" integer, "_Pang_" bigint, "_Cookie_" bigint, "_GM_ID_" integer, "_Flag_Gift_" integer, "_UCC_IMG_MARK_" character varying, "_Type_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT A.Msg_ID, A.fromid, A.message, A.giftdate, A.Contador_Vista, A.Lida_YN, COUNT(B.Msg_ID) OVER (PARTITION BY B.Msg_ID) AS CONTADOR, 
				(CASE WHEN b.item_id IS NULL THEN 0 ELSE b.item_id END) AS item_id, 
				(CASE WHEN b.item_typeid IS NULL THEN 0 ELSE b.item_typeid END) AS item_typeid, 
				(CASE WHEN b.Flag IS NULL THEN 0 ELSE b.Flag END) AS Flag, 
				(CASE WHEN b.Quantidade_item IS NULL THEN 0 ELSE b.Quantidade_item END) AS Quantidade_item, 
				(CASE WHEN b.Quantidade_Dia IS NULL THEN 0	ELSE b.Quantidade_Dia END) AS Quantidade_Dia, 
				(CASE WHEN b.Pang IS NULL THEN 0 ELSE b.Pang END) AS Pang, 
				(CASE WHEN b.Cookie IS NULL THEN 0 ELSE b.Cookie END) AS Cookie, 
				(CASE WHEN b.GM_ID IS NULL THEN 0 ELSE b.GM_ID END) AS GM_ID, 
				(CASE WHEN b.Flag_Gift IS NULL THEN 0 ELSE b.Flag_Gift END) AS Flag_Gift,
			   COALESCE((SELECT UCCIDX FROM pangya.tu_ucc WHERE item_id = B.item_id), '0') AS UCC_IMG_MARK, 
				(CASE WHEN b.Type IS NULL THEN 0 ELSE b.Type END) AS Type
		FROM pangya.pangya_gift_table A
		 LEFT OUTER JOIN
		 pangya.pangya_item_mail B
		 ON A.Msg_ID = B.Msg_ID AND B.valid = 1
		 WHERE A.valid = 1 AND A.Lida_YN = 0 AND A.UID = _IDUSER AND A.Msg_ID = _msg_id_in
		 ORDER BY A.Msg_ID DESC;
END;
$$;


ALTER FUNCTION pangya.procgetemailnaolidafrommailboxone(_iduser integer, _msg_id_in integer) OWNER TO postgres;

--
-- TOC entry 607 (class 1255 OID 23290)
-- Name: procgetfriendandguildmemberinfo(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetfriendandguildmemberinfo(_iduser integer) RETURNS TABLE("_NICK_" character varying, _uid_friend_ integer, _apelido_ character varying, _unknown1_ integer, _unknown2_ integer, _unknown3_ integer, _unknown4_ integer, _unknown5_ integer, _unknown6_ integer, _flag1_ integer, _state_flag_ integer, _level_ smallint, "_PLAYER_FLAG_" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDUSER > 0 THEN
	DECLARE 
		_GUILD_UID INT = (SELECT Guild_UID FROM pangya.account WHERE UID = _IDUSER);
	BEGIN

		RETURN QUERY SELECT (SELECT NICK FROM pangya.account WHERE UID = z.uid_friend) as NICK
				,z.uid_friend
				,y.apelido
				,CASE WHEN y.unknown1 IS NULL THEN -1 ELSE y.unknown1 END as unknown1 
				,CASE WHEN y.unknown2 IS NULL THEN 0 ELSE y.unknown2 END as unknown2
				,CASE WHEN y.unknown3 IS NULL THEN -1 ELSE y.unknown3 END as unknown3
				,CASE WHEN y.unknown4 IS NULL THEN 0 ELSE y.unknown4 END as unknown4
				,CASE WHEN y.unknown5 IS NULL THEN 0 ELSE y.unknown5 END as unknown5
				,CASE WHEN y.unknown6 IS NULL THEN 0 ELSE y.unknown6 END as unknown6
				,CASE WHEN y.flag1 IS NULL THEN -1 ELSE y.flag1 END as flag1
				,(CASE WHEN y.state_flag IS NULL THEN 0 ELSE y.state_flag END) + (SELECT Sex FROM pangya.account WHERE UID = z.uid_friend) as state_flag
				,(SELECT level FROM pangya.user_info WHERE UID = z.uid_friend) as level
				,CASE WHEN z._count = 2 OR y.uid_friend IS NULL THEN z._count + 1 ELSE z._count END as PLAYER_FLAG
		FROM
		(
			SELECT x.uid_friend, COUNT(x.uid_friend) as _count
			FROM
			(
				SELECT uid_friend
				FROM
					pangya.pangya_friend_list
				WHERE UID = _IDUSER
				UNION ALL
				(
					SELECT 
						b.MEMBER_UID as uid_friend
					FROM
						pangya.pangya_guild a
						INNER JOIN
						pangya.pangya_guild_member b
						ON a.GUILD_UID = b.GUILD_UID
					WHERE 
						a.GUILD_UID = _GUILD_UID
						AND b.MEMBER_STATE_FLAG < 9
						AND (a.GUILD_STATE NOT IN(4, 5) OR a.GUILD_CLOSURE_DATE IS NULL OR now() < a.GUILD_CLOSURE_DATE)
				)
			) x
			GROUP BY x.uid_friend
		) z
		LEFT OUTER JOIN
		pangya.pangya_friend_list y
		ON z.uid_friend = y.uid_friend AND y.UID = _IDUSER;
	END;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetfriendandguildmemberinfo(_iduser integer) OWNER TO postgres;

--
-- TOC entry 608 (class 1255 OID 23291)
-- Name: procgetfriendandguildmemberinfo_one(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetfriendandguildmemberinfo_one(_iduser integer, _friend_uid integer) RETURNS TABLE("_NICK_" character varying, _uid_friend_ integer, _apelido_ character varying, _unknown1_ integer, _unknown2_ integer, _unknown3_ integer, _unknown4_ integer, _unknown5_ integer, _unknown6_ integer, _flag1_ integer, _state_flag_ integer, _level_ smallint, "_PLAYER_FLAG_" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDUSER > 0 THEN
	DECLARE 
		_GUILD_UID INT = (SELECT Guild_UID FROM pangya.account WHERE UID = _IDUSER);
	BEGIN

		RETURN QUERY SELECT (SELECT NICK FROM pangya.account WHERE UID = z.uid_friend) as NICK
				,z.uid_friend
				,y.apelido
				,CASE WHEN y.unknown1 IS NULL THEN -1 ELSE y.unknown1 END as unknown1 
				,CASE WHEN y.unknown2 IS NULL THEN 0 ELSE y.unknown2 END as unknown2
				,CASE WHEN y.unknown3 IS NULL THEN -1 ELSE y.unknown3 END as unknown3
				,CASE WHEN y.unknown4 IS NULL THEN 0 ELSE y.unknown4 END as unknown4
				,CASE WHEN y.unknown5 IS NULL THEN 0 ELSE y.unknown5 END as unknown5
				,CASE WHEN y.unknown6 IS NULL THEN 0 ELSE y.unknown6 END as unknown6
				,CASE WHEN y.flag1 IS NULL THEN -1 ELSE y.flag1 END as flag1
				,(CASE WHEN y.state_flag IS NULL THEN 0 ELSE y.state_flag END) + (SELECT Sex FROM pangya.account WHERE UID = z.uid_friend) as state_flag
				,(SELECT level FROM pangya.user_info WHERE UID = z.uid_friend) as level
				,CASE WHEN z._count = 2 OR y.uid_friend IS NULL THEN z._count + 1 ELSE z._count END as PLAYER_FLAG
		FROM
		(
			SELECT x.uid_friend, COUNT(x.uid_friend) as _count
			FROM
			(
				SELECT uid_friend
				FROM
					pangya.pangya_friend_list
				WHERE UID = _IDUSER
				UNION ALL
				(
					SELECT 
						b.MEMBER_UID as uid_friend
					FROM
						pangya.pangya_guild a
						INNER JOIN
						pangya.pangya_guild_member b
						ON a.GUILD_UID = b.GUILD_UID
					WHERE 
						a.GUILD_UID = _GUILD_UID
						AND b.MEMBER_STATE_FLAG < 9
						AND (a.GUILD_STATE NOT IN(4, 5) OR a.GUILD_CLOSURE_DATE IS NULL OR now() < a.GUILD_CLOSURE_DATE)
				)
			) x
			GROUP BY x.uid_friend
		) z
		LEFT OUTER JOIN
		pangya.pangya_friend_list y
		ON z.uid_friend = y.uid_friend AND y.UID = _IDUSER
		WHERE  z.uid_friend = _FRIEND_UID;
	END;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetfriendandguildmemberinfo_one(_iduser integer, _friend_uid integer) OWNER TO postgres;

--
-- TOC entry 609 (class 1255 OID 23292)
-- Name: procgetfriendinfo(bigint); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetfriendinfo(_uid bigint) RETURNS TABLE(_uid_friend_ integer, _apelido_ character varying, _id_ character varying, _nick_ character varying, _sex_ smallint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT a.uid_friend, a.apelido, b.id, b.nick, b.sex FROM pangya.pangya_friend_list a INNER JOIN pangya.account b ON a.UID_FRIEND = b.UID WHERE a.UID = _UID;
END;
$$;


ALTER FUNCTION pangya.procgetfriendinfo(_uid bigint) OWNER TO postgres;

--
-- TOC entry 610 (class 1255 OID 23293)
-- Name: procgetgachajpconfig(); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetgachajpconfig() RETURNS TABLE(_rate_normal_ integer, _rate_rare_ integer, _gacha_num_ integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT rate_normal, rate_rare, gacha_num FROM pangya.pangya_gacha_jp_rate;
END;
$$;


ALTER FUNCTION pangya.procgetgachajpconfig() OWNER TO postgres;

--
-- TOC entry 612 (class 1255 OID 23294)
-- Name: procgetgiftclubset(integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetgiftclubset(_iduser integer, _iditem integer, _itemtypeid integer) RETURNS TABLE(_item_id_ bigint, "_UID_" integer, _typeid_ integer, "_Ano_" integer, "_C0_" smallint, "_C1_" smallint, "_C2_" smallint, "_C3_" smallint, "_C4_" smallint, "_Purchase_" smallint, "_Gift_flag_" smallint, "_Flag_" integer, "_APPLY_DATE_" bigint, "_END_DATE_" bigint, "_ItemType_" smallint, "_CARD_CHARACTER_1_" integer, "_CARD_CHARACTER_2_" integer, "_CARD_CHARACTER_3_" integer, "_CARD_CHARACTER_4_" integer, "_CARD_CADDIE_1_" integer, "_CARD_CADDIE_2_" integer, "_CARD_CADDIE_3_" integer, "_CARD_CADDIE_4_" integer, "_CARD_NPC_1_" integer, "_CARD_NPC_2_" integer, "_CARD_NPC_3_" integer, "_CARD_NPC_4_" integer, "_ClubSet_WorkShop_Flag_" smallint, "_ClubSet_WorkShop_C0_" smallint, "_ClubSet_WorkShop_C1_" smallint, "_ClubSet_WorkShop_C2_" smallint, "_ClubSet_WorkShop_C3_" smallint, "_ClubSet_WorkShop_C4_" smallint, "_Mastery_Pts_" integer, "_Recovery_Pts_" integer, "_Level_" integer, "_Up_" integer, "_SD_NAME_" character varying, "_SD_IDX_" character varying, "_SD_SEQ_" integer, "_SD_COPIER_NICK_" character varying, "_SD_COPIER_" integer, "_SD_TRADE_" integer, "_SD_FLAG_" integer, "_SD_STATUS_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDITEM > 0 AND _ITEMTYPEID > 0 THEN
		UPDATE pangya.pangya_item_warehouse SET UID = _IDUSER, valid = 1, Gift_flag = 0
		WHERE item_id = _IDITEM;

		UPDATE pangya.pangya_clubset_enchant SET UID = _IDUSER WHERE item_id = _IDITEM;

		RETURN QUERY SELECT * FROM pangya.ProcGetWareHouseItem_One(_IDUSER, _IDITEM);
	ELSE
		RETURN QUERY SELECT -1::bigint AS ITEM_ID,
					0::int as UID, 
					0::int as typeid, 
					0::int as Ano, 
					0::smallint as C0, 
					0::smallint as C1, 
					0::smallint as C2, 
					0::smallint as C3, 
					0::smallint as C4, 
					0::smallint as Purchase, 
					0::smallint as Gift_flag, 
					0::int as Flag, 
					0::bigint as APPLY_DATE, 
					0::bigint as END_DATE, 
					0::smallint as ItemType, 
					0::int as CARD_CHARACTER_1, 
					0::int as CARD_CHARACTER_2, 
					0::int as CARD_CHARACTER_3, 
					0::int as CARD_CHARACTER_4, 
					0::int as CARD_CADDIE_1, 
					0::int as CARD_CADDIE_2, 
					0::int as CARD_CADDIE_3, 
					0::int as CARD_CADDIE_4, 
					0::int as CARD_NPC_1, 
					0::int as CARD_NPC_2, 
					0::int as CARD_NPC_3, 
					0::int as CARD_NPC_4, 
					0::smallint as ClubSet_WorkShop_Flag, 
					0::smallint as ClubSet_WorkShop_C0, 
					0::smallint as ClubSet_WorkShop_C1, 
					0::smallint as ClubSet_WorkShop_C2, 
					0::smallint as ClubSet_WorkShop_C3, 
					0::smallint as ClubSet_WorkShop_C4, 
					0::int as Mastery_Pts, 
					0::int as Recovery_Pts, 
					0::int as Level, 
					0::int as Up, 
					null::varchar as SD_NAME, 
					null::varchar as SD_IDX, 
					0::int as SD_SEQ, 
					null::varchar as SD_COPIER_NICK, 
					0::int as SD_COPIER, 
					0::int as SD_TRADE, 
					0::int as SD_FLAG, 
					0::int as SD_STATUS;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetgiftclubset(_iduser integer, _iditem integer, _itemtypeid integer) OWNER TO postgres;

--
-- TOC entry 613 (class 1255 OID 23295)
-- Name: procgetgiftpart(integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetgiftpart(_iduser integer, _iditem integer, _itemtypeid integer, _type_iff integer) RETURNS TABLE(_item_id_ bigint, "_UID_" integer, _typeid_ integer, "_Ano_" integer, "_C0_" smallint, "_C1_" smallint, "_C2_" smallint, "_C3_" smallint, "_C4_" smallint, "_Purchase_" smallint, "_Gift_flag_" smallint, "_Flag_" integer, "_APPLY_DATE_" bigint, "_END_DATE_" bigint, "_ItemType_" smallint, "_CARD_CHARACTER_1_" integer, "_CARD_CHARACTER_2_" integer, "_CARD_CHARACTER_3_" integer, "_CARD_CHARACTER_4_" integer, "_CARD_CADDIE_1_" integer, "_CARD_CADDIE_2_" integer, "_CARD_CADDIE_3_" integer, "_CARD_CADDIE_4_" integer, "_CARD_NPC_1_" integer, "_CARD_NPC_2_" integer, "_CARD_NPC_3_" integer, "_CARD_NPC_4_" integer, "_ClubSet_WorkShop_Flag_" smallint, "_ClubSet_WorkShop_C0_" smallint, "_ClubSet_WorkShop_C1_" smallint, "_ClubSet_WorkShop_C2_" smallint, "_ClubSet_WorkShop_C3_" smallint, "_ClubSet_WorkShop_C4_" smallint, "_Mastery_Pts_" integer, "_Recovery_Pts_" integer, "_Level_" integer, "_Up_" integer, "_SD_NAME_" character varying, "_SD_IDX_" character varying, "_SD_SEQ_" integer, "_SD_COPIER_NICK_" character varying, "_SD_COPIER_" integer, "_SD_TRADE_" integer, "_SD_FLAG_" integer, "_SD_STATUS_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDITEM > 0 AND _ITEMTYPEID > 0 THEN
	
		UPDATE pangya.pangya_item_warehouse SET UID = _IDUSER, valid = 1, Gift_flag = 0
		WHERE item_id = _IDITEM;

		IF _TYPE_IFF = 8 OR _TYPE_IFF = 9 THEN
			UPDATE pangya.tu_ucc SET UID = _IDUSER WHERE ITEM_ID = _IDITEM;
		END IF;

		RETURN QUERY SELECT * FROM pangya.ProcGetWareHouseItem_One(_IDUSER, _IDITEM);
	ELSE
		RETURN QUERY SELECT -1::bigint AS ITEM_ID,
				0::int as UID, 
				0::int as typeid, 
				0::int as Ano, 
				0::smallint as C0, 
				0::smallint as C1, 
				0::smallint as C2, 
				0::smallint as C3, 
				0::smallint as C4, 
				0::smallint as Purchase, 
				0::smallint as Gift_flag, 
				0::int as Flag, 
				0::bigint as APPLY_DATE, 
				0::bigint as END_DATE, 
				0::smallint as ItemType, 
				0::int as CARD_CHARACTER_1, 
				0::int as CARD_CHARACTER_2, 
				0::int as CARD_CHARACTER_3, 
				0::int as CARD_CHARACTER_4, 
				0::int as CARD_CADDIE_1, 
				0::int as CARD_CADDIE_2, 
				0::int as CARD_CADDIE_3, 
				0::int as CARD_CADDIE_4, 
				0::int as CARD_NPC_1, 
				0::int as CARD_NPC_2, 
				0::int as CARD_NPC_3, 
				0::int as CARD_NPC_4, 
				0::smallint as ClubSet_WorkShop_Flag, 
				0::smallint as ClubSet_WorkShop_C0, 
				0::smallint as ClubSet_WorkShop_C1, 
				0::smallint as ClubSet_WorkShop_C2, 
				0::smallint as ClubSet_WorkShop_C3, 
				0::smallint as ClubSet_WorkShop_C4, 
				0::int as Mastery_Pts, 
				0::int as Recovery_Pts, 
				0::int as Level, 
				0::int as Up, 
				null::varchar as SD_NAME, 
				null::varchar as SD_IDX, 
				0::int as SD_SEQ, 
				null::varchar as SD_COPIER_NICK, 
				0::int as SD_COPIER, 
				0::int as SD_TRADE, 
				0::int as SD_FLAG, 
				0::int as SD_STATUS;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetgiftpart(_iduser integer, _iditem integer, _itemtypeid integer, _type_iff integer) OWNER TO postgres;

--
-- TOC entry 614 (class 1255 OID 23296)
-- Name: procgetgmgiftwebinfo(); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetgmgiftwebinfo() RETURNS TABLE("_GM_UID_" integer, "_PLAYER_UID_" integer, "_COUNT_ITEM_" bigint, _message_ character varying, "_REG_DATE_" character varying)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT 
		a.GM_UID,
		a.PLAYER_UID,
		COUNT(c.MSG_ID) as COUNT_ITEM,
		b.message,
		CAST(a.REG_DATE AS VARCHAR(19)) AS REG_DATE
	FROM 
		pangya.pangya_gm_gift_web_log a
		INNER JOIN
		pangya.pangya_gift_table b
		ON a.MSG_ID = b.Msg_ID
		LEFT OUTER JOIN
		pangya.pangya_item_mail c
		ON b.Msg_ID = c.Msg_ID
	GROUP BY a.MSG_ID, a.GM_UID, a.PLAYER_UID, b.message, a.REG_DATE
	ORDER BY a.REG_DATE;
END;
$$;


ALTER FUNCTION pangya.procgetgmgiftwebinfo() OWNER TO postgres;

--
-- TOC entry 615 (class 1255 OID 23297)
-- Name: procgetgoldentimeinfo(); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetgoldentimeinfo() RETURNS TABLE(_index_ integer, _type_ smallint, _begin_ date, _end_ date, _rate_ integer, _is_end_ smallint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT index, type, begin, "end", rate, is_end FROM pangya.pangya_golden_time_info WHERE is_end = 0;
END;
$$;


ALTER FUNCTION pangya.procgetgoldentimeinfo() OWNER TO postgres;

--
-- TOC entry 616 (class 1255 OID 23298)
-- Name: procgetgrandprixclear(bigint); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetgrandprixclear(_uid bigint) RETURNS TABLE(_typeid_ integer, _flag_ integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT typeid, flag FROM pangya.pangya_grandprix_clear WHERE UID = _UID;
END;
$$;


ALTER FUNCTION pangya.procgetgrandprixclear(_uid bigint) OWNER TO postgres;

--
-- TOC entry 617 (class 1255 OID 23299)
-- Name: procgetguildallnoticeinfo(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetguildallnoticeinfo(_player_uid integer) RETURNS TABLE("_TITLE_" character varying, "_TEXT_" character varying, "_NICKNAME_" character varying, "_REG_DATE_" character varying)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _PLAYER_UID > 0 THEN
	DECLARE 
		_OTHER_GUILD INT = (
			SELECT
				a.Guild_UID
			FROM
				pangya.account a
				INNER JOIN
				pangya.pangya_guild b
				ON a.Guild_UID = b.GUILD_UID
			WHERE a.UID = _PLAYER_UID AND 
				(b.GUILD_STATE NOT IN(4, 5) OR b.GUILD_CLOSURE_DATE IS NULL OR now() < b.GUILD_CLOSURE_DATE)
		);
	BEGIN

		-- Player está em uma guild válida
		IF _OTHER_GUILD IS NOT NULL THEN
		DECLARE 
			_GUILD_UID INT = 0;
		BEGIN

			_GUILD_UID = (SELECT Guild_UID FROM pangya.account WHERE UID = _PLAYER_UID);

			IF _GUILD_UID IS NOT NULL AND _GUILD_UID > 0 THEN

				RETURN QUERY SELECT 
					b.TITLE,
					b.TEXT,
					(SELECT nick FROM pangya.account WHERE uid = b.OWNER_UID) AS NICKNAME,
					CAST(b.REG_DATE AS VARCHAR(19)) AS REG_DATE
				FROM 
					pangya.pangya_guild a
					INNER JOIN
					pangya.pangya_guild_notice b
					ON a.GUILD_UID = b.GUILD_UID
				WHERE a.GUILD_UID = _GUILD_UID AND b.STATE = 1
				ORDER BY b.REG_DATE DESC;

			END IF;
		END;
		END IF;

	END;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetguildallnoticeinfo(_player_uid integer) OWNER TO postgres;

--
-- TOC entry 618 (class 1255 OID 23300)
-- Name: procgetguildbbsinfo(bigint); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetguildbbsinfo(_bbs_seq bigint) RETURNS TABLE("_SEQ_" bigint, "_TITLE_" character varying, "_TEXT_" character varying, "_TYPE_" smallint, "_OWNER_UID_" integer, "_NICKNAME_" character varying, "_VIEWS_" bigint, "_REG_DATE_" character varying)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _BBS_SEQ > 0 THEN

		-- Update VIEWS BBS
		UPDATE pangya.pangya_guild_bbs SET VIEWS = VIEWS + 1 WHERE SEQ = _BBS_SEQ AND STATE = 1;
		
		-- BBS
		RETURN QUERY SELECT
			a.SEQ,
			a.TITLE,
			a.TEXT,
			a.TYPE,
			a.OWNER_UID,
			(SELECT nick FROM pangya.account WHERE UID = a.OWNER_UID) AS NICKNAME,
			a.VIEWS,
			CAST(a.REG_DATE AS VARCHAR(19)) AS REG_DATE
		FROM
			pangya.pangya_guild_bbs a
		WHERE a.SEQ = _BBS_SEQ AND a.STATE = 1;

	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetguildbbsinfo(_bbs_seq bigint) OWNER TO postgres;

--
-- TOC entry 595 (class 1255 OID 23301)
-- Name: procgetguildbbsreplyinfo(bigint); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetguildbbsreplyinfo(_bbs_seq bigint) RETURNS TABLE("_SEQ_" bigint, "_TEXT_" character varying, "_OWNER_UID_" integer, "_NICKNAME_" character varying, "_REG_DATE_" character varying)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _BBS_SEQ > 0 THEN

		-- Reply
		RETURN QUERY SELECT
			b.SEQ,
			b.TEXT,
			b.OWNER_UID,
			(SELECT nick FROM pangya.account WHERE UID = b.OWNER_UID) AS NICKNAME,
			CAST(b.REG_DATE AS VARCHAR(19)) AS REG_DATE
		FROM
			pangya.pangya_guild_bbs a
			INNER JOIN
			pangya.pangya_guild_bbs_res b
			ON a.SEQ = b.BBS_SEQ
		WHERE a.SEQ = _BBS_SEQ AND a.STATE = 1 AND b.STATE = 1
		ORDER BY b.REG_DATE;

	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetguildbbsreplyinfo(_bbs_seq bigint) OWNER TO postgres;

--
-- TOC entry 619 (class 1255 OID 23302)
-- Name: procgetguildinfo(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetguildinfo(_iduser integer, _opt integer) RETURNS TABLE("_GUILD_UID_" bigint, "_GUILD_NAME_" character varying, "_GUILD_NUM_MEMBER_" bigint, "_GUILD_MARK_IMG_" character varying, "_GUILD_MARK_IMG_IDX_" integer, "_GUILD_NOTICE_" character varying, "_GUILD_INFO_" character varying, "_GUILD_POINT_" bigint, "_GUILD_PANG_" bigint, "_GUILD_MEMBER_IDENTITY_" integer, "_GUILD_LEADER_" integer, "_GUILD_LEADER_NICK_" character varying, "_GUILD_REG_DATE_" timestamp without time zone)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDUSER > 0 THEN

		RETURN QUERY SELECT 
			a.GUILD_UID, 
			a.GUILD_NAME, 
			(SELECT count(*) FROM pangya.pangya_guild_member WHERE GUILD_UID = a.GUILD_UID AND MEMBER_STATE_FLAG < 9) AS GUILD_NUM_MEMBER, 
			(CASE WHEN b.MEMBER_STATE_FLAG < 9 THEN a.GUILD_MARK_IMG ELSE '' END) AS GUILD_MARK_IMG,
			(CASE WHEN b.MEMBER_STATE_FLAG < 9 THEN a.GUILD_MARK_IMG_IDX ELSE 0 END) AS GUILD_MARK_IMG_IDX,
			a.GUILD_NOTICE, 
			a.GUILD_INFO,
			(CASE WHEN _opt = 1 THEN a.GUILD_POINT ELSE b.GUILD_POINT END) AS GUILD_POINT,
			(CASE WHEN _opt = 1 THEN a.GUILD_PANG ELSE b.GUILD_PANG END) AS GUILD_PANG, 
			b.MEMBER_STATE_FLAG AS GUILD_MEMBER_IDENTITY, 
			a.GUILD_LEADER, 
			(SELECT account.NICK FROM pangya.account WHERE account.UID = a.GUILD_LEADER) AS GUILD_LEADER_NICK, 
			a.GUILD_REG_DATE
		FROM 
		pangya.pangya_guild AS a
		INNER JOIN 
		pangya.pangya_guild_member AS b
		ON a.GUILD_UID = b.GUILD_UID 
			AND b.MEMBER_STATE_FLAG < 9
			AND (a.GUILD_STATE NOT IN(4, 5) OR a.GUILD_CLOSURE_DATE IS NULL OR now() < a.GUILD_CLOSURE_DATE)
		INNER JOIN
		pangya.account AS c
		ON b.MEMBER_UID = c.UID
		WHERE 
			c.UID = _IDUSER;

	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetguildinfo(_iduser integer, _opt integer) OWNER TO postgres;

--
-- TOC entry 620 (class 1255 OID 23303)
-- Name: procgetguildinfocomplete(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetguildinfocomplete(_uid integer) RETURNS TABLE("_GUILD_UID_" bigint, "_GUILD_NAME_" character varying, "_GUILD_MASTER_" character varying, "_GUILD_SUB_MASTER_" character varying, "_GUILD_MEMBERS_" bigint, "_GUILD_STATE_" smallint, "_GUILD_FLAG_" smallint, "_GUILD_CONDITION_LEVEL_" smallint, "_GUILD_PERMITION_JOIN_" smallint, "_GUILD_REG_DATE_" character varying, "_GUILD_INFO_" character varying, "_GUILD_NOTICE_" character varying, "_GUILD_MARK_IMG_IDX_" integer, "_GUILD_NEW_MARK_IDX_" integer, "_GUILD_INTRO_IMG_" character varying, "_GUILD_CLOSURE_DATE_" character varying)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT 
		a.GUILD_UID, 
		a.GUILD_NAME, 
		(SELECT NICK FROM pangya.account WHERE UID = a.GUILD_LEADER) AS GUILD_MASTER,
		COALESCE((SELECT NICK FROM pangya.account WHERE UID = a.GUILD_SUB_MASTER), '') AS GUILD_SUB_MASTER,
		(SELECT count(GUILD_UID) FROM pangya.pangya_guild_member WHERE GUILD_UID = a.GUILD_UID AND MEMBER_STATE_FLAG < 9) AS GUILD_MEMBERS,
		a.GUILD_STATE,
		a.GUILD_FLAG,
		a.GUILD_CONDITION_LEVEL,
		a.GUILD_PERMITION_JOIN,
		CAST(a.GUILD_REG_DATE AS VARCHAR(10)) AS GUILD_REG_DATE,
		a.GUILD_INFO,
		a.GUILD_NOTICE,
		a.GUILD_MARK_IMG_IDX,
		a.GUILD_NEW_MARK_IDX,
		a.GUILD_INTRO_IMG,
		CAST(a.GUILD_CLOSURE_DATE AS VARCHAR(19)) AS GUILD_CLOSURE_DATE
	FROM
		pangya.pangya_guild a
		WHERE a.GUILD_UID = _UID
			AND (a.GUILD_STATE NOT IN(4, 5) OR a.GUILD_CLOSURE_DATE IS NULL OR now() < a.GUILD_CLOSURE_DATE);
END;
$$;


ALTER FUNCTION pangya.procgetguildinfocomplete(_uid integer) OWNER TO postgres;

--
-- TOC entry 621 (class 1255 OID 23304)
-- Name: procgetguildinfonew(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetguildinfonew(_guild_uid integer) RETURNS TABLE("_GUILD_UID_" bigint, "_GUILD_NAME_" character varying, "_GUILD_MASTER_" integer, "_GUILD_MASTER_NICKNAME_" character varying, "_GUILD_CONDITION_LEVEL_" smallint, "_GUILD_STATE_" smallint, "_GUILD_FLAG_" smallint, "_GUILD_PERMITION_JOIN_" smallint, "_GUILD_MEMBERS_" bigint, "_GUILD_REG_DATE_" character varying)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT
         a.GUILD_UID, 
         a.GUILD_NAME, 
         a.GUILD_LEADER AS GUILD_MASTER, 
		 (SELECT NICK FROM pangya.account WHERE UID = a.GUILD_LEADER) AS GUILD_MASTER_NICKNAME,
		 a.GUILD_CONDITION_LEVEL,
		 a.GUILD_STATE,
		 a.GUILD_FLAG,
		 a.GUILD_PERMITION_JOIN,
		 (SELECT count(GUILD_UID) FROM pangya.pangya_guild_member WHERE GUILD_UID = a.GUILD_UID AND MEMBER_STATE_FLAG < 9) AS GUILD_MEMBERS,
         CAST(a.GUILD_REG_DATE AS VARCHAR(10)) AS GUILD_REG_DATE
      FROM 
	  pangya.pangya_guild a
      WHERE a.GUILD_UID = _GUILD_UID
		AND (a.GUILD_STATE NOT IN(4, 5) OR a.GUILD_CLOSURE_DATE IS NULL OR now() < a.GUILD_CLOSURE_DATE)
		LIMIT 1;
END;
$$;


ALTER FUNCTION pangya.procgetguildinfonew(_guild_uid integer) OWNER TO postgres;

--
-- TOC entry 622 (class 1255 OID 23305)
-- Name: procgetguildmatchlist(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetguildmatchlist(_guild_uid integer, _page integer) RETURNS TABLE("_LINHAS_" bigint, "_GUILD_1_UID_" integer, "_GUILD_1_NAME_" character varying, "_GUILD_1_MARK_INDEX_" integer, "_GUILD_1_POINT_" integer, "_GUILD_1_PANG_" integer, "_GUILD_2_UID_" integer, "_GUILD_2_NAME_" character varying, "_GUILD_2_MARK_INDEX_" integer, "_GUILD_2_POINT_" integer, "_GUILD_2_PANG_" integer, "_REG_DATE_" character varying)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_LINHAS BIGINT = 0;
BEGIN
	IF _GUILD_UID > 0 AND _PAGE >= 0 THEN
	DECLARE
		_OTHER_GUILD INT = (
			SELECT
				b.GUILD_UID
			FROM
				pangya.pangya_guild b
			WHERE b.GUILD_UID = _GUILD_UID
				AND (b.GUILD_STATE NOT IN(4, 5) OR b.GUILD_CLOSURE_DATE IS NULL OR now() < b.GUILD_CLOSURE_DATE)
		);
	BEGIN

		-- Está em uma Guild válida
		IF _OTHER_GUILD IS NOT NULL THEN
		
			-- Total de linhas da consulta
			SELECT
				COUNT(a.index) INTO _LINHAS
			FROM
				pangya.pangya_guild_match a
			WHERE a.GUILD_1_UID = _GUILD_UID OR a.guild_2_uid = _GUILD_UID;

			RETURN QUERY SELECT
				_LINHAS AS LINHAS,
				a.GUILD_1_UID,
				(SELECT GUILD_NAME FROM pangya.pangya_guild WHERE GUILD_UID = a.GUILD_1_UID) AS GUILD_1_NAME,
				(SELECT GUILD_MARK_IMG_IDX FROM pangya.pangya_guild WHERE GUILD_UID = a.GUILD_1_UID) AS GUILD_1_MARK_INDEX,
				a.GUILD_1_POINT,
				a.GUILD_1_PANG,
				a.GUILD_2_UID,
				(SELECT GUILD_NAME FROM pangya.pangya_guild WHERE GUILD_UID = a.GUILD_2_UID) AS GUILD_2_NAME,
				(SELECT GUILD_MARK_IMG_IDX FROM pangya.pangya_guild WHERE GUILD_UID = a.GUILD_2_UID) AS GUILD_2_MARK_INDEX,
				a.GUILD_2_POINT,
				a.GUILD_2_PANG,
				CAST(a.REG_DATE AS VARCHAR(16)) AS REG_DATE
			FROM
				pangya.pangya_guild_match a
			WHERE a.GUILD_1_UID = _GUILD_UID OR a.guild_2_uid = _GUILD_UID
			ORDER BY a.reg_date DESC
			OFFSET (_PAGE * 13) ROWS
			FETCH NEXT 13 ROWS ONLY;

		END IF;

	END;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetguildmatchlist(_guild_uid integer, _page integer) OWNER TO postgres;

--
-- TOC entry 623 (class 1255 OID 23306)
-- Name: procgetguildmemberinfo(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetguildmemberinfo(_uid_guild integer) RETURNS TABLE("_GUILD_UID_" bigint, "_GUILD_NAME_" character varying, "_GUILD_PANG_" integer, "_GUILD_POINT_" integer, "_GUILD_MEMBER_IDENTITY_" integer, "_GUILD_MEMBER_UID_" integer, "_GUILD_MEMBER_MSG_" character varying, "_GUILD_MEMBER_NICK_" character varying, "_GUILD_MEMBER_ON_" smallint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _UID_GUILD > 0 THEN

      RETURN QUERY SELECT 
         a.GUILD_UID, 
         a.GUILD_NAME, 
         b.GUILD_PANG, 
         b.GUILD_POINT, 
         b.MEMBER_STATE_FLAG AS GUILD_MEMBER_IDENTITY, 
         b.MEMBER_UID AS GUILD_MEMBER_UID, 
         b.MEMBER_MSG AS GUILD_MEMBER_MSG, 
         c.NICK AS GUILD_MEMBER_NICK, 
         c.Logon AS GUILD_MEMBER_ON
      FROM 
		pangya.pangya_guild AS a
		INNER JOIN
		pangya.pangya_guild_member AS b
		ON a.GUILD_UID = b.GUILD_UID
		INNER JOIN
		pangya.account AS c
		ON b.MEMBER_UID = c.UID
      WHERE 
         a.GUILD_UID = _UID_GUILD
		 AND (b.MEMBER_STATE_FLAG < 9)
		 AND (a.GUILD_STATE NOT IN(4, 5) OR a.GUILD_CLOSURE_DATE IS NULL OR now() < a.GUILD_CLOSURE_DATE);

	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetguildmemberinfo(_uid_guild integer) OWNER TO postgres;

--
-- TOC entry 624 (class 1255 OID 23307)
-- Name: procgetguildmemberlist(integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetguildmemberlist(_uid integer, _page integer, _opt integer, _max_line_page integer) RETURNS TABLE("_LINHAS_" bigint, "_MEMBER_UID_" integer, "_MEMBER_STATE_FLAG_" integer, "_NICKNAME_" character varying, "_REG_DATE_" character varying)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_LINHAS BIGINT = 0;
BEGIN
	IF _MAX_LINE_PAGE <= 0 THEN
		_MAX_LINE_PAGE = 14;
	END IF;
	
	IF _UID > 0 AND _PAGE >= 0 THEN
	DECLARE
		_OTHER_GUILD INT = (
			SELECT
				b.GUILD_UID
			FROM
				pangya.pangya_guild b
			WHERE b.GUILD_UID = _UID
				AND (b.GUILD_STATE NOT IN(4, 5) OR b.GUILD_CLOSURE_DATE IS NULL OR now() < b.GUILD_CLOSURE_DATE)
		);
	BEGIN

		-- Está em uma Guild válida
		IF _OTHER_GUILD IS NOT NULL THEN
		
			-- Total de linhas da consulta
			SELECT 
				COUNT(b.MEMBER_UID) INTO _LINHAS
			FROM 
				pangya.pangya_guild a
				INNER JOIN
				pangya.pangya_guild_member b
				ON a.GUILD_UID = b.GUILD_UID
			WHERE a.GUILD_UID = _UID
				AND (_OPT = 0 OR (_OPT = 1 AND b.MEMBER_STATE_FLAG = 9) OR (_OPT = 2 AND b.MEMBER_STATE_FLAG < 9));

			RETURN QUERY SELECT 
				_LINHAS as LINHAS,
				b.MEMBER_UID,
				b.MEMBER_STATE_FLAG,
				(SELECT nick FROM pangya.account WHERE uid = b.MEMBER_UID) AS NICKNAME,
				CAST(b.REG_DATE AS VARCHAR(10)) AS REG_DATE
			FROM 
				pangya.pangya_guild a
				INNER JOIN
				pangya.pangya_guild_member b
				ON a.GUILD_UID = b.GUILD_UID
			WHERE a.GUILD_UID = _UID
				AND (_OPT = 0 OR (_OPT = 1 AND b.MEMBER_STATE_FLAG = 9) OR (_OPT = 2 AND b.MEMBER_STATE_FLAG < 9))
			ORDER BY b.MEMBER_STATE_FLAG, b.REG_DATE
			OFFSET (_PAGE * _MAX_LINE_PAGE) ROWS
			FETCH NEXT _MAX_LINE_PAGE ROWS ONLY;

		END IF;
	
	END;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetguildmemberlist(_uid integer, _page integer, _opt integer, _max_line_page integer) OWNER TO postgres;

--
-- TOC entry 625 (class 1255 OID 23308)
-- Name: procgetguildnewscreate(); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetguildnewscreate() RETURNS TABLE("_GUILD_UID_" bigint, "_GUILD_NAME_" character varying, "_GUILD_INFO_" character varying, "_GUILD_MARK_IMG_IDX_" integer, "_GUILD_REG_DATE_" date)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT 
		GUILD_UID, 
		GUILD_NAME, 
		GUILD_INFO, 
		GUILD_MARK_IMG_IDX, 
		CAST(GUILD_REG_DATE AS DATE) AS GUILD_REG_DATE 
	FROM 
		pangya.pangya_guild 
	WHERE GUILD_ACCEPT_DATE IS NOT NULL AND CAST(now() AS DATE) = CAST(GUILD_ACCEPT_DATE AS DATE) AND GUILD_STATE >= 1 AND GUILD_MARK_IMG_IDX > 0
		AND (GUILD_STATE NOT IN(4, 5) OR GUILD_CLOSURE_DATE IS NULL OR now() < GUILD_CLOSURE_DATE)
	ORDER BY GUILD_REG_DATE DESC;
END;
$$;


ALTER FUNCTION pangya.procgetguildnewscreate() OWNER TO postgres;

--
-- TOC entry 626 (class 1255 OID 23309)
-- Name: procgetguildnoticeinfo(bigint); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetguildnoticeinfo(_notice_seq bigint) RETURNS TABLE("_SEQ_" bigint, "_TITLE_" character varying, "_TEXT_" character varying, "_OWNER_UID_" integer, "_NICKNAME_" character varying, "_VIEWS_" bigint, "_REG_DATE_" character varying)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _NOTICE_SEQ > 0 THEN
	DECLARE
		_OTHER_GUILD INT = (
			SELECT
				a.GUILD_UID
			FROM
				pangya.pangya_guild_notice a
				INNER JOIN
				pangya.pangya_guild b
				ON a.GUILD_UID = b.GUILD_UID
			WHERE a.SEQ = _NOTICE_SEQ
				AND (b.GUILD_STATE NOT IN(4, 5) OR b.GUILD_CLOSURE_DATE IS NULL OR now() < b.GUILD_CLOSURE_DATE)
		);
	BEGIN

		-- Está em uma Guild válida
		IF _OTHER_GUILD IS NOT NULL THEN

			-- Update VIEWS BBS
			UPDATE pangya.pangya_guild_notice SET VIEWS = VIEWS + 1 WHERE SEQ = _NOTICE_SEQ AND STATE = 1;
		
			-- BBS
			RETURN QUERY SELECT
				a.SEQ,
				a.TITLE,
				a.TEXT,
				a.OWNER_UID,
				(SELECT nick FROM pangya.account WHERE UID = a.OWNER_UID) AS NICKNAME,
				a.VIEWS,
				CAST(a.REG_DATE AS VARCHAR(19)) AS REG_DATE
			FROM
				pangya.pangya_guild_notice a
			WHERE a.SEQ = _NOTICE_SEQ AND a.STATE = 1;

		END IF;
	
	END;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetguildnoticeinfo(_notice_seq bigint) OWNER TO postgres;

--
-- TOC entry 627 (class 1255 OID 23310)
-- Name: procgetguildnoticelist(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetguildnoticelist(_uid integer, _page integer) RETURNS TABLE("_LINHAS_" bigint, "_SEQ_" bigint, "_TITLE_" character varying, "_NICKNAME_" character varying, "_REG_DATE_" character varying)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_LINHAS BIGINT = 0;
BEGIN
	IF _PAGE >= 0 AND _UID > 0 THEN
	DECLARE 
		_OTHER_GUILD INT = (
			SELECT
				b.GUILD_UID
			FROM
				pangya.pangya_guild b
			WHERE b.GUILD_UID = _UID
				AND (b.GUILD_STATE NOT IN(4, 5) OR b.GUILD_CLOSURE_DATE IS NULL OR now() < b.GUILD_CLOSURE_DATE)
		);
	BEGIN

		-- Está em uma Guild válida
		IF _OTHER_GUILD IS NOT NULL THEN
		
			-- Total de linhas da consulta
			SELECT 
				COUNT(b.SEQ) INTO _LINHAS
			FROM 
				pangya.pangya_guild a
				INNER JOIN
				pangya.pangya_guild_notice b
				ON a.GUILD_UID = b.GUILD_UID
			WHERE a.GUILD_UID = _UID AND b.STATE = 1;
		
			RETURN QUERY SELECT 
				_LINHAS AS LINHAS,
				b.SEQ,
				b.TITLE,
				(SELECT nick FROM pangya.account WHERE uid = b.OWNER_UID) AS NICKNAME,
				CAST(b.REG_DATE AS VARCHAR(10)) AS REG_DATE
			FROM 
				pangya.pangya_guild a
				INNER JOIN
				pangya.pangya_guild_notice b
				ON a.GUILD_UID = b.GUILD_UID
			WHERE a.GUILD_UID = _UID AND b.STATE = 1
			ORDER BY b.REG_DATE DESC
			OFFSET (_PAGE * 12) ROWS
			FETCH NEXT 12 ROWS ONLY;

		END IF;

	END;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetguildnoticelist(_uid integer, _page integer) OWNER TO postgres;

--
-- TOC entry 628 (class 1255 OID 23311)
-- Name: procgetguildprivatebbsinfo(bigint); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetguildprivatebbsinfo(_bbs_seq bigint) RETURNS TABLE("_SEQ_" bigint, "_TITLE_" character varying, "_TEXT_" character varying, "_OWNER_UID_" integer, "_NICKNAME_" character varying, "_VIEWS_" bigint, "_REG_DATE_" character varying)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _BBS_SEQ > 0 THEN
	DECLARE
		_OTHER_GUILD INT = (
			SELECT
				a.GUILD_UID
			FROM
				pangya.pangya_guild_private_bbs a
				INNER JOIN
				pangya.pangya_guild b
				ON a.GUILD_UID = b.GUILD_UID
			WHERE a.SEQ = _BBS_SEQ
				AND (b.GUILD_STATE NOT IN(4, 5) OR b.GUILD_CLOSURE_DATE IS NULL OR now() < b.GUILD_CLOSURE_DATE)
		);
	BEGIN

		-- Está em uma Guild válida
		IF _OTHER_GUILD IS NOT NULL THEN

			-- Update VIEWS BBS
			UPDATE pangya.pangya_guild_private_bbs SET VIEWS = VIEWS + 1 WHERE SEQ = _BBS_SEQ AND STATE = 1;
		
			-- BBS
			RETURN QUERY SELECT
				a.SEQ,
				a.TITLE,
				a.TEXT,
				a.OWNER_UID,
				(SELECT nick FROM pangya.account WHERE UID = a.OWNER_UID) AS NICKNAME,
				a.VIEWS,
				CAST(a.REG_DATE AS VARCHAR(19)) AS REG_DATE
			FROM
				pangya.pangya_guild_private_bbs a
			WHERE a.SEQ = _BBS_SEQ AND a.STATE = 1;

		END IF;

	END;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetguildprivatebbsinfo(_bbs_seq bigint) OWNER TO postgres;

--
-- TOC entry 630 (class 1255 OID 23312)
-- Name: procgetguildprivatebbslist(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetguildprivatebbslist(_uid integer, _page integer) RETURNS TABLE("_LINHAS_" bigint, "_SEQ_" bigint, "_RES_COUNT_" bigint, "_TITLE_" character varying, "_NICKNAME_" character varying, "_REG_DATE_" character varying)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_LINHAS BIGINT = 0;
BEGIN
	IF _UID > 0 AND _PAGE >= 0 THEN
	DECLARE
		_OTHER_GUILD INT = (
			SELECT
				b.GUILD_UID
			FROM
				pangya.pangya_guild b
			WHERE b.GUILD_UID = _UID
				AND (b.GUILD_STATE NOT IN(4, 5) OR b.GUILD_CLOSURE_DATE IS NULL OR now() < b.GUILD_CLOSURE_DATE)
		);
	BEGIN

		-- Está em uma Guild válida
		IF _OTHER_GUILD IS NOT NULL THEN
		
			-- Total de linhas da consulta
			SELECT 
				COUNT(b.SEQ) INTO _LINHAS
			FROM 
				pangya.pangya_guild a
				INNER JOIN
				pangya.pangya_guild_private_bbs b
				ON a.GUILD_UID = b.GUILD_UID
			WHERE a.GUILD_UID = _UID AND b.STATE = 1;

			RETURN QUERY SELECT 
				_LINHAS AS LINHAS,
				b.SEQ,
				(SELECT COUNT(SEQ) FROM pangya.pangya_guild_private_bbs_res WHERE GUILD_BBS_SEQ = b.SEQ AND STATE = 1) AS RES_COUNT,
				b.TITLE,
				(SELECT nick FROM pangya.account WHERE uid = b.OWNER_UID) AS NICKNAME,
				CAST(b.REG_DATE AS VARCHAR(10)) AS REG_DATE
			FROM 
				pangya.pangya_guild a
				INNER JOIN
				pangya.pangya_guild_private_bbs b
				ON a.GUILD_UID = b.GUILD_UID
			WHERE a.GUILD_UID = _UID AND b.STATE = 1
			ORDER BY b.REG_DATE DESC
			OFFSET (_PAGE * 15) ROWS
			FETCH NEXT 15 ROWS ONLY;

		END IF;

	END;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetguildprivatebbslist(_uid integer, _page integer) OWNER TO postgres;

--
-- TOC entry 631 (class 1255 OID 23313)
-- Name: procgetguildprivatebbsreplyinfo(bigint); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetguildprivatebbsreplyinfo(_bbs_seq bigint) RETURNS TABLE("_SEQ_" bigint, "_TEXT_" character varying, "_OWNER_UID_" integer, "_NICKNAME_" character varying, "_REG_DATE_" character varying)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _BBS_SEQ > 0 THEN
	DECLARE
		_OTHER_GUILD INT = (
			SELECT
				a.GUILD_UID
			FROM
				pangya.pangya_guild_private_bbs a
				INNER JOIN
				pangya.pangya_guild b
				ON a.GUILD_UID = b.GUILD_UID
			WHERE a.SEQ = _BBS_SEQ
				AND (b.GUILD_STATE NOT IN(4, 5) OR b.GUILD_CLOSURE_DATE IS NULL OR now() < b.GUILD_CLOSURE_DATE)
		);
	BEGIN

		-- Está em uma Guild válida
		IF _OTHER_GUILD IS NOT NULL THEN
		
			-- Reply
			RETURN QUERY SELECT
				b.SEQ,
				b.TEXT,
				b.OWNER_UID,
				(SELECT nick FROM pangya.account WHERE UID = b.OWNER_UID) AS NICKNAME,
				CAST(b.REG_DATE AS VARCHAR(19)) AS REG_DATE
			FROM
				pangya.pangya_guild_private_bbs a
				INNER JOIN
				pangya.pangya_guild_private_bbs_res b
				ON a.SEQ = b.GUILD_BBS_SEQ
			WHERE a.SEQ = _BBS_SEQ AND a.STATE = 1 AND b.STATE = 1
			ORDER BY b.REG_DATE;

		END IF;

	END;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetguildprivatebbsreplyinfo(_bbs_seq bigint) OWNER TO postgres;

--
-- TOC entry 632 (class 1255 OID 23314)
-- Name: procgetguildranking(integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetguildranking(_page integer, _search text) RETURNS TABLE("_LINHAS_" bigint, "_REG_DATE_" character varying, "_RANK_" integer, "_GUILD_UID_" bigint, "_GUILD_NAME_" character varying, "_GUILD_WIN_" integer, "_GUILD_LOSE_" integer, "_GUILD_DRAW_" integer, "_GUILD_POINT_" bigint, "_GUILD_PANG_" bigint, "_LAST_RANK_" integer, "_GUILD_MARK_IMG_IDX_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_LINHAS BIGINT = 0;
	_REG_DATE VARCHAR(10) = NULL;
BEGIN
	IF _PAGE >= 0 THEN
	
		-- Total de linhas da consulta
		SELECT COUNT(a.GUILD_UID), CAST(MAX(b.REG_DATE) AS VARCHAR(10)) INTO _LINHAS, _REG_DATE 
		FROM 
			pangya.pangya_guild a
			INNER JOIN
			pangya.pangya_guild_ranking b
			ON a.GUILD_UID = b.GUILD_UID
		WHERE 
			lower(a.GUILD_NAME) LIKE N'%' || lower(_SEARCH) || N'%'
			AND (a.GUILD_STATE NOT IN(4, 5) OR a.GUILD_CLOSURE_DATE IS NULL OR now() < a.GUILD_CLOSURE_DATE);

		RETURN QUERY SELECT 
			_LINHAS AS LINHAS,
			_REG_DATE AS REG_DATE,
			a.RANK,
			a.GUILD_UID,
			a.GUILD_NAME,
			a.GUILD_WIN,
			a.GUILD_LOSE,
			a.GUILD_DRAW,
			a.GUILD_POINT,
			a.GUILD_PANG,
			a.LAST_RANK,
			a.GUILD_MARK_IMG_IDX
		FROM
			(
				SELECT	
					b.RANK,
					b.LAST_RANK,
					a.GUILD_UID,
					a.GUILD_NAME,
					a.GUILD_WIN,
					a.GUILD_LOSE,
					a.GUILD_DRAW,
					a.GUILD_POINT,
					a.GUILD_PANG,
					a.GUILD_MARK_IMG_IDX,
					b.REG_DATE
				FROM 
					pangya.pangya_guild a
					INNER JOIN
					pangya.pangya_guild_ranking b
					ON a.GUILD_UID = b.GUILD_UID
				WHERE 
					lower(a.GUILD_NAME) LIKE N'%' || lower(_SEARCH) || N'%'
					AND (a.GUILD_STATE NOT IN(4, 5) OR a.GUILD_CLOSURE_DATE IS NULL OR now() < a.GUILD_CLOSURE_DATE)
			) a
		ORDER BY a.RANK
			OFFSET (_PAGE * 10) ROWS
			FETCH NEXT 10 ROWS ONLY;

	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetguildranking(_page integer, _search text) OWNER TO postgres;

--
-- TOC entry 633 (class 1255 OID 23315)
-- Name: procgetguildrankingupdatetime(); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetguildrankingupdatetime() RETURNS TABLE("_REG_DATE" character varying)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT 
		CAST(MAX(REG_DATE) AS VARCHAR(19)) AS REG_DATE
	FROM 
		pangya.pangya_guild_ranking;
END;
$$;


ALTER FUNCTION pangya.procgetguildrankingupdatetime() OWNER TO postgres;

--
-- TOC entry 634 (class 1255 OID 23316)
-- Name: procgetguildrssinfo(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetguildrssinfo(_player_uid integer) RETURNS TABLE("_RSS_VER_" bigint, "_TITLE_" character varying, "_TEXT_" character varying, "_NICKNAME_" character varying, "_REG_DATE_" character varying)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _PLAYER_UID > 0 THEN
	DECLARE 
		_OTHER_GUILD INT = (
			SELECT
				a.Guild_UID
			FROM
				pangya.account a
				INNER JOIN
				pangya.pangya_guild b
				ON a.Guild_UID = b.GUILD_UID
			WHERE a.UID = _PLAYER_UID AND 
				(b.GUILD_STATE NOT IN(4, 5) OR b.GUILD_CLOSURE_DATE IS NULL OR now() < b.GUILD_CLOSURE_DATE)
		);
	BEGIN

		-- Player está em uma guild válida
		IF _OTHER_GUILD IS NOT NULL THEN
		DECLARE
			_GUILD_UID INT = 0;
		BEGIN

			_GUILD_UID = (SELECT Guild_UID FROM pangya.account WHERE UID = _PLAYER_UID);

			IF _GUILD_UID IS NOT NULL AND _GUILD_UID > 0 THEN
			DECLARE
				_RSS_VER BIGINT = 0;
			BEGIN
				
				-- RSS Version is last sequence id Notice
				SELECT
					b.SEQ INTO _RSS_VER
				FROM
					pangya.pangya_guild a
					INNER JOIN
					pangya.pangya_guild_notice b
					ON a.GUILD_UID = b.GUILD_UID
				WHERE a.GUILD_UID = _GUILD_UID AND b.STATE = 1
				ORDER BY b.REG_DATE DESC;

				-- Last 5 Notice
				RETURN QUERY SELECT 
					_RSS_VER as RSS_VER,
					b.TITLE,
					b.TEXT,
					(SELECT nick FROM pangya.account WHERE uid = b.OWNER_UID) AS NICKNAME,
					CAST(b.REG_DATE AS VARCHAR(19)) AS REG_DATE
				FROM 
					pangya.pangya_guild a
					INNER JOIN
					pangya.pangya_guild_notice b
					ON a.GUILD_UID = b.GUILD_UID
				WHERE a.GUILD_UID = _GUILD_UID AND b.STATE = 1
				ORDER BY b.REG_DATE DESC
				OFFSET 0 ROWS
				FETCH NEXT 5 ROWS ONLY;

			END;
			END IF;

		END;
		END IF;

	END;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetguildrssinfo(_player_uid integer) OWNER TO postgres;

--
-- TOC entry 635 (class 1255 OID 23317)
-- Name: procgetguildupdateactivity(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetguildupdateactivity(_guild_uid integer, _member_uid integer) RETURNS TABLE("_INDEX_" bigint, "_GUILD_UID_" integer, "_OWNER_UPDATE_" integer, "_PLAYER_UID_" integer, "_TYPE_UPDATE_" smallint, "_REG_DATE_" timestamp without time zone)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _GUILD_UID > 0 THEN

		RETURN QUERY SELECT
			INDEX,
			GUILD_UID,
			OWNER_UPDATE,	-- Quem fez a Ação
			PLAYER_UID,
			TYPE_UPDATE,
			REG_DATE
		FROM
			pangya.pangya_guild_update_activity
		WHERE
			GUILD_UID = _GUILD_UID 
			AND OWNER_UPDATE = _MEMBER_UID
			AND STATE = 0
		ORDER BY REG_DATE;

	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetguildupdateactivity(_guild_uid integer, _member_uid integer) OWNER TO postgres;

--
-- TOC entry 636 (class 1255 OID 23318)
-- Name: procgetinfoplayertoweb(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetinfoplayertoweb(_type integer) RETURNS TABLE(_uid_ bigint, _id_ character varying, _nick_ character varying, _level_ smallint, _idstate_ bigint, _cap_ integer, _pang_ numeric, _cookie_ numeric, _channel_ smallint, _lobby_ smallint, _room_ smallint, _place_ smallint, "_logonCount_" bigint, _counter_ integer, _userip_ character varying, "_LastLogonTime_" character varying, _logon_ smallint, _game_server_id_ character varying, _name_ character varying)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _TYPE = 0 THEN -- Default Todas as contas que já fizeram logon pelo menos uma vez
		
		RETURN QUERY SELECT 
				a.uid, 
				a.id, 
				a.nick, 
				c.level, 
				a.idstate, 
				a.capability as cap, 
				c.pang, 
				c.cookie, 
				d.channel, 
				d.lobby, 
				d.room, 
				d.place, 
				a.logonCount, 
				e.counter,
				a.userip,
				CAST(a.LastLogonTime AS VARCHAR(16)) AS LastLogonTime, 
				a.logon, 
				a.game_server_id, 
				b.name 
		FROM pangya.account a, pangya.user_info c, pangya.pangya_server_list b, pangya.pangya_player_location d, pangya.pangya_attendance_reward e
		WHERE a.game_server_id::bigint = b.uid  AND a.uid = c.uid AND a.uid = d.uid AND a.UID = e.UID;

	ELSIF _TYPE = 1 THEN -- Todas as contas que não fizeram login

		RETURN QUERY SELECT 
				a.uid, 
				a.id, 
				a.nick, 
				c.level, 
				a.idstate, 
				a.capability as cap, 
				c.pang, 
				c.cookie, 
				d.channel, 
				d.lobby, 
				d.room, 
				d.place, 
				a.logonCount,
				e.counter,
				a.userip,
				CAST(a.LastLogonTime AS VARCHAR(16)) AS LastLogonTime, 
				a.logon, 
				a.game_server_id, 
				'unknown'::varchar(50) as name 
		FROM pangya.account a, pangya.user_info c, pangya.pangya_player_location d, pangya.pangya_attendance_reward e
		WHERE  a.uid = c.uid AND a.uid = d.uid AND a.UID = e.UID AND (a.first_login = 0 OR a.first_set = 0);

	ELSIF _TYPE = 2 THEN -- Todas as contas online no momento
		
		RETURN QUERY SELECT 
				a.uid, 
				a.id, 
				a.nick, 
				c.level, 
				a.idstate, 
				a.capability as cap, 
				c.pang, c.cookie, 
				d.channel, 
				d.lobby, 
				d.room, 
				d.place, 
				a.logonCount, 
				e.counter,
				a.userip,
				CAST(a.LastLogonTime AS VARCHAR(16)) AS LastLogonTime,
				a.logon, 
				a.game_server_id, 
				b.name 
		FROM pangya.account a, pangya.user_info c, pangya.pangya_server_list b, pangya.pangya_player_location d, pangya.pangya_attendance_reward e
		WHERE a.logon = 1 AND a.game_server_id::bigint = b.uid AND a.uid = c.uid AND a.uid = d.uid AND a.UID = e.UID;

	ELSIF _TYPE = 3 THEN -- Todas as contas que foram enviado o email de cadastro pode ter as que terminaram o cadatro e as que ainda não terminaram
	
		RETURN QUERY SELECT 
			a.uid, 
			a.nome, 
			a.sobre_nome, 
			a.email, 
			a.key_uniq, 
			a.finish_reg, 
			a.ip_register, a.date_reg 
		FROM pangya.contas_beta a;

	ELSIF _TYPE = 4 THEN -- Todas as contas banidas
		
		RETURN QUERY SELECT 
				a.uid, 
				a.id, 
				a.nick, 
				c.level, 
				a.idstate, 
				a.capability as cap, 
				c.pang, 
				c.cookie, 
				d.channel, 
				d.lobby, 
				d.room, 
				d.place, 
				a.logonCount,
				e.counter,
				a.userip,
				CAST(a.LastLogonTime AS VARCHAR(16)) AS LastLogonTime, 
				a.logon, 
				a.game_server_id, 
				COALESCE(b.Name, 'unknown')::varchar(50) as name 
		FROM 
			pangya.account a
			INNER JOIN
			pangya.user_info c
			ON a.uid = c.uid
			INNER JOIN
			pangya.pangya_player_location d
			ON a.uid = d.uid
			INNER JOIN
			pangya.pangya_attendance_reward e
			ON a.UID = e.UID
			LEFT OUTER JOIN
			pangya.pangya_server_list b
			ON b.UID = a.game_server_id::bigint
		WHERE 
			(a.IDState = 1 AND COALESCE((a.BlockRegDate + ('1 minute'::interval * a.BlockTime)), now()) > now() OR a.IDState > 1);
	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetinfoplayertoweb(_type integer) OWNER TO postgres;

--
-- TOC entry 637 (class 1255 OID 23319)
-- Name: procgetinformationemail(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetinformationemail(_iduser integer, _idmsg integer) RETURNS TABLE("_Msg_ID_" bigint, _fromid_ character varying, _giftdate_ timestamp without time zone, _message_ character varying, "_Lida_YN_" smallint, "_ITEM_ID_" integer, "_ITEM_TYPEID_" integer, "_FLAG_" integer, "_QUANTIDADE_ITEM_" integer, "_QUANTIDADE_DIA_" integer, "_PANG_" bigint, "_COOKIE_" bigint, "_GM_ID_" integer, "_FLAG_GIFT_" integer, "_UCC_IMG_MARK_" character varying, "_TYPE_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	UPDATE pangya.pangya_gift_table
		SET 
			Lida_YN = 1,
			Contador_Vista = Contador_Vista + 1
	WHERE pangya_gift_table.UID = _IDUSER AND pangya_gift_table.Msg_ID = _IDMSG;
	
	RETURN QUERY SELECT 
         X.Msg_ID, 
         X.fromid, 
         X.giftdate, 
         X.message, 
         X.Lida_YN, 
         (CASE 
            WHEN X.item_id IS NULL THEN 0
            ELSE X.item_id
         END) AS ITEM_ID, 
         (CASE 
            WHEN X.item_typeid IS NULL THEN 0
            ELSE X.item_typeid
         END) AS ITEM_TYPEID, 
         (CASE 
            WHEN X.Flag IS NULL THEN 0
            ELSE X.Flag
         END) AS FLAG, 
         (CASE 
            WHEN X.Quantidade_item IS NULL THEN 0
            ELSE X.Quantidade_item
         END) AS QUANTIDADE_ITEM, 
         (CASE 
            WHEN X.Quantidade_Dia IS NULL THEN 0
            ELSE X.Quantidade_Dia
         END) AS QUANTIDADE_DIA, 
         (CASE 
            WHEN X.Pang IS NULL THEN 0
            ELSE X.Pang
         END) AS PANG, 
         (CASE 
            WHEN X.Cookie IS NULL THEN 0
            ELSE X.Cookie
         END) AS COOKIE, 
         (CASE 
            WHEN X.GM_ID IS NULL THEN 0
            ELSE X.GM_ID
         END) AS GM_ID, 
         (CASE 
            WHEN X.Flag_Gift IS NULL THEN 0
            ELSE X.Flag_Gift
         END) AS FLAG_GIFT, 
         (CASE 
            WHEN X.UCC_IMG_MARK IS NULL THEN '0'
            ELSE X.UCC_IMG_MARK
         END) AS UCC_IMG_MARK, 
         (CASE 
            WHEN X.Type IS NULL THEN 0
            ELSE X.Type
         END) AS TYPE
      FROM 
         (
            SELECT 
               A.UID, 
               A.Msg_ID, 
               A.fromid, 
               A.message, 
               A.giftdate, 
               A.Contador_Vista, 
               A.Lida_YN, 
               A.Flag AS Flag_MSG, 
               B.ID_MSG, 
               B.item_id, 
               B.item_typeid, 
               B.Flag, 
               B.Quantidade_item, 
               B.Quantidade_Dia, 
               B.GET_DATE, 
               B.Pang, 
               B.Cookie, 
               B.GM_ID, 
               B.Flag_Gift,
			   COALESCE((SELECT UCCIDX FROM pangya.tu_ucc WHERE item_id = B.item_id), '0') AS UCC_IMG_MARK, 
               B.Type
            FROM 
               (
                  SELECT 
                     pangya_gift_table.UID, 
                     pangya_gift_table.Msg_ID, 
                     pangya_gift_table.fromid, 
                     pangya_gift_table.message, 
                     pangya_gift_table.giftdate, 
                     pangya_gift_table.Flag, 
                     pangya_gift_table.Contador_Vista, 
                     pangya_gift_table.Lida_YN
                  FROM pangya.pangya_gift_table
                  WHERE pangya_gift_table.valid = 1
               )  AS A 
                  LEFT OUTER JOIN 
                  (
                     SELECT 
                        pangya_item_mail.Msg_ID AS ID_MSG, 
                        pangya_item_mail.item_id, 
                        pangya_item_mail.item_typeid, 
                        pangya_item_mail.Flag, 
                        pangya_item_mail.Quantidade_item, 
                        pangya_item_mail.Quantidade_Dia, 
                        pangya_item_mail.GET_DATE, 
                        pangya_item_mail.Pang, 
                        pangya_item_mail.Cookie, 
                        pangya_item_mail.GM_ID, 
                        pangya_item_mail.Flag_Gift, 
                        pangya_item_mail.UCC_IMG_MARK, 
                        pangya_item_mail.Type
                     FROM pangya.pangya_item_mail
                     WHERE pangya_item_mail.valid = 1
                  )  AS B 
                  ON A.Msg_ID = B.ID_MSG
         )  AS X
      WHERE X.UID = _IDUSER AND X.Msg_ID = _IDMSG;
END;
$$;


ALTER FUNCTION pangya.procgetinformationemail(_iduser integer, _idmsg integer) OWNER TO postgres;

--
-- TOC entry 638 (class 1255 OID 23320)
-- Name: procgetinformationemail2(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetinformationemail2(_iduser integer, _idmsg integer) RETURNS TABLE("_Msg_ID_" bigint, _fromid_ character varying, _giftdate_ timestamp without time zone, _message_ character varying, "_Contador_Vista_" integer, "_Lida_YN_" smallint, "_ITEM_ID_" integer, "_ITEM_TYPEID_" integer, "_FLAG_" integer, "_QUANTIDADE_ITEM_" integer, "_QUANTIDADE_DIA_" integer, "_PANG_" bigint, "_COOKIE_" bigint, "_GM_ID_" integer, "_FLAG_GIFT_" integer, "_UCC_IMG_MARK_" character varying, "_TYPE_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDUSER > 0 AND _IDMSG > 0 THEN

		RETURN QUERY SELECT 
            A.Msg_ID, 
            A.fromid,  
			A.giftdate, 
            A.message, 
            A.Contador_Vista, 
            A.Lida_YN, 
            COALESCE(B.item_id, 0) AS ITEM_ID,
            COALESCE(B.item_typeid, 0) AS ITEM_TYPEID, 
            COALESCE(B.Flag, 0) AS FLAG, 
			COALESCE(B.Quantidade_item, 0) AS QUANTIDADE_ITEM, 
			COALESCE(B.Quantidade_Dia, 0) AS QUANTIDADE_DIA, 
            COALESCE(B.Pang, 0) AS PANG, 
			COALESCE(B.Cookie, 0) AS COOKIE, 
			COALESCE(B.GM_ID, 0) AS GM_ID, 
			COALESCE(B.Flag_Gift, 0) AS FLAG_GIFT,  
			COALESCE((SELECT UCCIDX FROM pangya.tu_ucc WHERE item_id = B.item_id), '0') AS UCC_IMG_MARK, 
            COALESCE(B.Type, 0) AS TYPE
        FROM 
        (
            SELECT 
				UID, 
				Msg_ID, 
				fromid, 
				message, 
				giftdate, 
				Flag, 
				Contador_Vista, 
				Lida_YN
            FROM pangya.pangya_gift_table
            WHERE valid = 1
        )  AS A 
        LEFT OUTER JOIN 
        (
            SELECT 
				Msg_ID AS ID_MSG, 
				item_id, 
				item_typeid, 
				Flag, 
				Quantidade_item, 
				Quantidade_Dia, 
				Pang, 
				Cookie, 
				GM_ID, 
				Flag_Gift, 
				UCC_IMG_MARK, 
				Type
            FROM pangya.pangya_item_mail
            WHERE valid = 1
        )  AS B 
        ON A.Msg_ID = B.ID_MSG
		WHERE UID = _IDUSER AND Msg_ID = _IDMSG;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetinformationemail2(_iduser integer, _idmsg integer) OWNER TO postgres;

--
-- TOC entry 639 (class 1255 OID 23321)
-- Name: procgetiptable(); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetiptable() RETURNS TABLE(_ip_ character varying, _mask_ character varying, _date_ timestamp without time zone, _index_ bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT 
		ip, mask, date, index 
	FROM 
		pangya.pangya_ip_table
	ORDER BY date;
END;
$$;


ALTER FUNCTION pangya.procgetiptable() OWNER TO postgres;

--
-- TOC entry 640 (class 1255 OID 23322)
-- Name: procgetitemboostpangflag(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetitemboostpangflag(_iduser integer) RETURNS TABLE("_FLAG_RET_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_FLAG_RET int = 0;
BEGIN
	
	IF 
	(
		SELECT pangya_item_warehouse.typeid
		FROM pangya.pangya_item_warehouse
		WHERE 
			pangya_item_warehouse.UID = _IDUSER AND 
			pangya_item_warehouse.valid = 1 AND 
			pangya_item_warehouse.C0 > 0 AND 
			pangya_item_warehouse.typeid = 436207617
	) IS NOT NULL THEN
		_FLAG_RET = _FLAG_RET | 1;
	END IF;

	IF 
	(
		SELECT pangya_item_warehouse.typeid
		FROM pangya.pangya_item_warehouse
		WHERE 
			pangya_item_warehouse.UID = _IDUSER AND 
			pangya_item_warehouse.valid = 1 AND 
			pangya_item_warehouse.C0 > 0 AND 
			pangya_item_warehouse.typeid = 436207618
	) IS NOT NULL THEN
		_FLAG_RET = _FLAG_RET | 1;
	END IF;

	IF 
	(
		SELECT pangya_item_warehouse.typeid
		FROM pangya.pangya_item_warehouse
		WHERE 
			pangya_item_warehouse.UID = _IDUSER AND 
			pangya_item_warehouse.valid = 1 AND 
			pangya_item_warehouse.C0 > 0 AND 
			pangya_item_warehouse.typeid = 436207621
	) IS NOT NULL THEN
		_FLAG_RET = _FLAG_RET | 2;
	END IF;

	RETURN QUERY SELECT _FLAG_RET;
END;
$$;


ALTER FUNCTION pangya.procgetitemboostpangflag(_iduser integer) OWNER TO postgres;

--
-- TOC entry 641 (class 1255 OID 23323)
-- Name: procgetitembuff(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetitembuff(_iduser integer) RETURNS TABLE(_index_ bigint, _typeid_ integer, _reg_date_ timestamp without time zone, _end_date_ timestamp without time zone, _tipo_ smallint, _percent_ integer, _use_yn_ smallint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	
	RETURN QUERY SELECT index, typeid, reg_date, end_date, tipo, percent, use_yn
	FROM pangya.pangya_item_buff
	WHERE UID = _IDUSER AND use_yn = 1 AND datediff('second', now()::timestamp, end_date) > 0;
END;
$$;


ALTER FUNCTION pangya.procgetitembuff(_iduser integer) OWNER TO postgres;

--
-- TOC entry 642 (class 1255 OID 23324)
-- Name: procgetiteminfo(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetiteminfo(_iduser integer, _iditem integer) RETURNS TABLE(_item_id_ bigint, "_UID_" integer, _typeid_ integer, "_C0_" smallint, "_C1_" smallint, "_C2_" smallint, "_C3_" smallint, "_C4_" smallint, "_Purchase_" smallint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	
	IF _IDITEM < 0 THEN
         SELECT pangya_user_equip.club_id INTO _IDITEM
         FROM pangya.pangya_user_equip
         WHERE pangya_user_equip.UID = _IDUSER;
	END IF;

    RETURN QUERY SELECT 
         pangya_item_warehouse.item_id, 
         pangya_item_warehouse.UID, 
         pangya_item_warehouse.typeid, 
         pangya_item_warehouse.C0, 
         pangya_item_warehouse.C1, 
         pangya_item_warehouse.C2, 
         pangya_item_warehouse.C3, 
         pangya_item_warehouse.C4, 
         pangya_item_warehouse.Purchase
      FROM pangya.pangya_item_warehouse
      WHERE 
         pangya_item_warehouse.UID = _IDUSER AND 
         pangya_item_warehouse.valid = 1 AND 
         pangya_item_warehouse.item_id = _IDITEM;
END;
$$;


ALTER FUNCTION pangya.procgetiteminfo(_iduser integer, _iditem integer) OWNER TO postgres;

--
-- TOC entry 603 (class 1255 OID 23325)
-- Name: procgetitenspangrate(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetitenspangrate(_iduser integer) RETURNS TABLE("_PANG_RATE_ITEM_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_qntd int = 0;
BEGIN
	
	_qntd = _qntd + public.GetCardPangRate(_IDUSER);

	_qntd = _qntd + public.GetItensPangRate(_IDUSER);
	
	RETURN QUERY SELECT _qntd AS PANG_RATE_ITEM;
END;
$$;


ALTER FUNCTION pangya.procgetitenspangrate(_iduser integer) OWNER TO postgres;

--
-- TOC entry 629 (class 1255 OID 23326)
-- Name: procgetitenssametypeiddiffid(integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetitenssametypeiddiffid(_iduser integer, __typeid integer, __limit integer) RETURNS TABLE(_item_id_ bigint, _typeid_ integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_qntd int = 0;
BEGIN
	
	RETURN QUERY SELECT pangya_item_warehouse.item_id, pangya_item_warehouse.typeid
      FROM pangya.pangya_item_warehouse
      WHERE 
         pangya_item_warehouse.UID = _IDUSER AND 
         pangya_item_warehouse.valid = 1 AND 
         pangya_item_warehouse.Gift_flag <> 1 AND 
         pangya_item_warehouse.typeid = __typeid AND 
         pangya_item_warehouse.item_id NOT IN 
         (
            SELECT td_char_equip_s4.ITEMID
            FROM pangya.td_char_equip_s4
            WHERE 
               td_char_equip_s4.UID = _IDUSER AND 
               td_char_equip_s4.USE_YN = 'Y' AND 
               td_char_equip_s4.EQUIP_TYPE = __typeid
         )
         ORDER BY pangya_item_warehouse.item_id
		 LIMIT __limit;
END;
$$;


ALTER FUNCTION pangya.procgetitenssametypeiddiffid(_iduser integer, __typeid integer, __limit integer) OWNER TO postgres;

--
-- TOC entry 643 (class 1255 OID 23327)
-- Name: procgetlastplayergame(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetlastplayergame(_iduser integer) RETURNS TABLE("_SEX_0_" integer, "_NICK_0_" character varying, "_ID_0_" character varying, "_UID_0_" integer, "_SEX_1_" integer, "_NICK_1_" character varying, "_ID_1_" character varying, "_UID_1_" integer, "_SEX_2_" integer, "_NICK_2_" character varying, "_ID_2_" character varying, "_UID_2_" integer, "_SEX_3_" integer, "_NICK_3_" character varying, "_ID_3_" character varying, "_UID_3_" integer, "_SEX_4_" integer, "_NICK_4_" character varying, "_ID_4_" character varying, "_UID_4_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	
	RETURN QUERY SELECT 
         pangya_last_players_user.SEX_0, 
         pangya_last_players_user.NICK_0, 
         pangya_last_players_user.ID_0, 
         pangya_last_players_user.UID_0, 
         pangya_last_players_user.SEX_1, 
         pangya_last_players_user.NICK_1, 
         pangya_last_players_user.ID_1, 
         pangya_last_players_user.UID_1, 
         pangya_last_players_user.SEX_2, 
         pangya_last_players_user.NICK_2, 
         pangya_last_players_user.ID_2, 
         pangya_last_players_user.UID_2, 
         pangya_last_players_user.SEX_3, 
         pangya_last_players_user.NICK_3, 
         pangya_last_players_user.ID_3, 
         pangya_last_players_user.UID_3, 
         pangya_last_players_user.SEX_4, 
         pangya_last_players_user.NICK_4, 
         pangya_last_players_user.ID_4, 
         pangya_last_players_user.UID_4
      FROM pangya.pangya_last_players_user
      WHERE pangya_last_players_user.UID = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.procgetlastplayergame(_iduser integer) OWNER TO postgres;

--
-- TOC entry 644 (class 1255 OID 23328)
-- Name: procgetlegacytikishopinfo(bigint); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetlegacytikishopinfo(_uid bigint) RETURNS TABLE("_Tiki_Points_" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT Tiki_Points FROM pangya.pangya_tiki_points WHERE UID = _UID;
END;
$$;


ALTER FUNCTION pangya.procgetlegacytikishopinfo(_uid bigint) OWNER TO postgres;

--
-- TOC entry 645 (class 1255 OID 23329)
-- Name: procgetlevelmemorial(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetlevelmemorial(_iduser integer) RETURNS TABLE("_LEVEL_" integer, "_ACHIEVEMENT_POINT_" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	
	RETURN QUERY SELECT public.GetLevelMemorial(_IDUSER) AS LEVEL, public.GetAchievementPoint(_IDUSER) AS ACHIVEMENT_POINT;
END;
$$;


ALTER FUNCTION pangya.procgetlevelmemorial(_iduser integer) OWNER TO postgres;

--
-- TOC entry 646 (class 1255 OID 23330)
-- Name: procgetlistgachasystemplayeritens(integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetlistgachasystemplayeritens(_uid integer, _page integer, _type integer, _type_item integer) RETURNS TABLE("_LINHAS_" bigint, "_ID_" bigint, "_TYPEID_" integer, "_QNTD_" bigint, "_NAME_" character varying)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_LINHAS BIGINT = 0;
BEGIN
	IF _UID > 0 AND _PAGE >= 0 THEN

		-- Total de linhas da consulta
		SELECT 
			COUNT(a.index) INTO _LINHAS
		FROM 
			pangya.pangya_gacha_jp_player_win a
			INNER JOIN
			pangya.pangya_gacha_jp_all_item_list b
			ON a.typeid = b.typeid
		WHERE a.UID = _UID AND a.valid = 1 AND a.send_mail = 0
			AND (_TYPE = 1 -- Type 1 retorna todos os itens
			OR (_TYPE = 2 AND a.rarity_type = 0 AND ((_TYPE_ITEM = 100 AND b.char_type > 10/*Spika*/) OR _TYPE_ITEM = b.char_type)) -- Type 2 só retorna os raros, com o seu char_type
			OR (_TYPE = 3 AND a.rarity_type != 0 AND ((_TYPE_ITEM = 1 AND b.char_type = 11/*Aztec*/) OR (_TYPE_ITEM = 2 AND b.char_type != 11/*Aztec*/)))); -- Itens e Aztec

		RETURN QUERY SELECT 
			_LINHAS as LINHAS,
			a.index AS ID,
			a.typeid AS TYPEID,
			a.qnty AS QNTD,
			b.name AS NAME
		FROM 
			pangya.pangya_gacha_jp_player_win a
			INNER JOIN
			pangya.pangya_gacha_jp_all_item_list b
			ON a.typeid = b.typeid
		WHERE a.UID = _UID AND a.valid = 1 AND a.send_mail = 0
			AND (_TYPE = 1 -- Type 1 retorna todos os itens
			OR (_TYPE = 2 AND a.rarity_type = 0 AND ((_TYPE_ITEM = 100 AND b.char_type > 10/*Spika*/) OR _TYPE_ITEM = b.char_type)) -- Type 2 só retorna os raros, com o seu char_type
			OR (_TYPE = 3 AND a.rarity_type != 0 AND ((_TYPE_ITEM = 1 AND b.char_type = 11/*Aztec*/) OR (_TYPE_ITEM = 2 AND b.char_type != 11/*Aztec*/)))) -- Itens e Aztec
		ORDER BY b.REG_DATE DESC
		OFFSET (_PAGE * 50) ROWS
		FETCH NEXT 50 ROWS ONLY;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetlistgachasystemplayeritens(_uid integer, _page integer, _type integer, _type_item integer) OWNER TO postgres;

--
-- TOC entry 647 (class 1255 OID 23331)
-- Name: procgetlistguild(integer, integer, text, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetlistguild(_page integer, _category integer, _search text, _sort integer) RETURNS TABLE("_LINHAS_" bigint, "_GUILD_UID_" bigint, "_GUILD_NAME_" character varying, "_GUILD_MASTER_" character varying, "_MEMBERS_" bigint, "_GUILD_CONDITION_LEVEL_" smallint, "_GUILD_STATE_" smallint, "_GUILD_FLAG_" smallint, "_GUILD_MARK_IMG_IDX_" integer, "_GUILD_REG_DATE_" character varying)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_LINHAS BIGINT = 0;
BEGIN
	IF _PAGE >= 0 THEN

		IF _CATEGORY = 0 THEN	-- GUILD NAME
		
			-- Total de linhas da consulta
			SELECT COUNT(a.GUILD_UID) INTO _LINHAS
			FROM 
			pangya.pangya_guild a
			INNER JOIN
			pangya.account b
			ON a.GUILD_LEADER = b.uid
			WHERE 
				lower(a.GUILD_NAME) LIKE N'%' || lower(_SEARCH) || N'%'
				AND (a.GUILD_STATE NOT IN(4, 5) OR a.GUILD_CLOSURE_DATE IS NULL OR now() < a.GUILD_CLOSURE_DATE);

			RETURN QUERY SELECT 
				_LINHAS as LINHAS,
				a.GUILD_UID,
				a.GUILD_NAME,
				a.GUILD_MASTER,
				a.MEMBERS,
				a.GUILD_CONDITION_LEVEL,
				a.GUILD_STATE,
				a.GUILD_FLAG,
				a.GUILD_MARK_IMG_IDX,
				CAST(a.GUILD_REG_DATE as VARCHAR(10)) as GUILD_REG_DATE
			FROM
				(
					SELECT	a.*,
							(SELECT COUNT(GUILD_UID) FROM pangya.pangya_guild_member WHERE GUILD_UID = a.GUILD_UID AND MEMBER_STATE_FLAG < 9) as MEMBERS,
							b.NICK as GUILD_MASTER
					FROM 
					pangya.pangya_guild a
					INNER JOIN
					pangya.account b
					ON a.GUILD_LEADER = b.uid
					WHERE 
						lower(a.GUILD_NAME) LIKE N'%' || lower(_SEARCH) || N'%'
						AND (a.GUILD_STATE NOT IN(4, 5) OR a.GUILD_CLOSURE_DATE IS NULL OR now() < a.GUILD_CLOSURE_DATE)
				) a
			ORDER BY
				CASE WHEN _SORT = 0 THEN a.GUILD_NAME ELSE null END,
				CASE WHEN _SORT = 1 THEN a.GUILD_MASTER ELSE null END,
				CASE WHEN _SORT = 2 THEN a.MEMBERS ELSE null END DESC,
				CASE WHEN _SORT = 3 THEN a.GUILD_CONDITION_LEVEL ELSE null END,
				CASE WHEN _SORT = 4 THEN a.GUILD_STATE ELSE null END,
				CASE WHEN _SORT = 5 THEN a.GUILD_FLAG ELSE null END,
				CASE WHEN _SORT = 6 THEN CAST(a.GUILD_REG_DATE as timestamp) ELSE CAST(a.GUILD_REG_DATE as timestamp) END DESC
				OFFSET (_PAGE * 12) ROWS
				FETCH NEXT 12 ROWS ONLY;

		ELSE	-- GUILD MASTER NICKNAME
			
			-- Total de linhas da consulta
			SELECT COUNT(a.GUILD_UID) INTO _LINHAS
			FROM 
			pangya.pangya_guild a
			INNER JOIN
			pangya.account b
			ON a.GUILD_LEADER = b.uid
			WHERE 
				lower(b.NICK) LIKE N'%' || lower(_SEARCH) || N'%'
				AND (a.GUILD_STATE NOT IN(4, 5) OR a.GUILD_CLOSURE_DATE IS NULL OR now() < a.GUILD_CLOSURE_DATE);
			
			RETURN QUERY SELECT 
				_LINHAS as LINHAS,
				a.GUILD_UID,
				a.GUILD_NAME,
				a.GUILD_MASTER,
				a.MEMBERS,
				a.GUILD_CONDITION_LEVEL,
				a.GUILD_STATE,
				a.GUILD_FLAG,
				a.GUILD_MARK_IMG_IDX,
				CAST(a.GUILD_REG_DATE as VARCHAR(10)) as GUILD_REG_DATE
			FROM
				(
					SELECT	a.*,
							(SELECT COUNT(GUILD_UID) FROM pangya.pangya_guild_member WHERE GUILD_UID = a.GUILD_UID AND MEMBER_STATE_FLAG < 9) as MEMBERS,
							b.NICK as GUILD_MASTER
					FROM 
					pangya.pangya_guild a
					INNER JOIN
					pangya.account b
					ON a.GUILD_LEADER = b.uid
					WHERE 
						lower(b.NICK) LIKE N'%' || lower(_SEARCH) || N'%'
						AND (a.GUILD_STATE NOT IN(4, 5) OR a.GUILD_CLOSURE_DATE IS NULL OR now() < a.GUILD_CLOSURE_DATE)
				) a
			ORDER BY
				CASE WHEN _SORT = 0 THEN a.GUILD_NAME ELSE null END,
				CASE WHEN _SORT = 1 THEN a.GUILD_MASTER ELSE null END,
				CASE WHEN _SORT = 2 THEN a.MEMBERS ELSE null END DESC,
				CASE WHEN _SORT = 3 THEN a.GUILD_CONDITION_LEVEL ELSE null END,
				CASE WHEN _SORT = 4 THEN a.GUILD_STATE ELSE null END,
				CASE WHEN _SORT = 5 THEN a.GUILD_FLAG ELSE null END,
				CASE WHEN _SORT = 6 THEN CAST(a.GUILD_REG_DATE as timestamp) ELSE CAST(a.GUILD_REG_DATE as timestamp) END DESC
				OFFSET (_PAGE * 12) ROWS
				FETCH NEXT 12 ROWS ONLY;

		END IF;	-- ELSE IFF CATEGORY

	END IF; -- CHECK PAGE
END;
$$;


ALTER FUNCTION pangya.procgetlistguild(_page integer, _category integer, _search text, _sort integer) OWNER TO postgres;

--
-- TOC entry 648 (class 1255 OID 23332)
-- Name: procgetlistguildbbs(integer, integer, text, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetlistguildbbs(_page integer, _category integer, _search text, _sort integer) RETURNS TABLE("_LINHAS_" bigint, "_SEQ_" bigint, "_TYPE_" smallint, "_TITLE_" character varying, "_RES_COUNT_" bigint, "_NICKNAME_" character varying, "_REG_DATE_" character varying)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_LINHAS BIGINT = 0;
BEGIN
	IF _PAGE >= 0 THEN
	
		-- Total de linhas da consulta
		SELECT 
			COUNT(a.SEQ) INTO _LINHAS
		FROM 
			pangya.pangya_guild_bbs a
		WHERE 
			STATE = 1 AND (_CATEGORY = 10 OR a.TYPE = _CATEGORY)
			AND lower(a.TITLE) LIKE N'%' || lower(_SEARCH) || N'%';

		RETURN QUERY SELECT
			_LINHAS AS LINHAS,
			a.SEQ,
			a.TYPE,
			a.TITLE,
			a.RES_COUNT,
			a.NICKNAME,
			CAST(a.REG_DATE AS VARCHAR(10)) AS REG_DATE
		FROM
			(
				SELECT 
					a.SEQ,
					a.TYPE,
					a.TITLE,
					(SELECT COUNT(SEQ) FROM pangya.pangya_guild_bbs_res WHERE BBS_SEQ = a.SEQ AND STATE = 1) AS RES_COUNT,
					(SELECT nick FROM pangya.account WHERE UID = a.OWNER_UID) AS NICKNAME,
					a.REG_DATE
				FROM 
					pangya.pangya_guild_bbs a
				WHERE 
					STATE = 1 AND (_CATEGORY = 10 OR a.TYPE = _CATEGORY)
					AND lower(a.TITLE) LIKE N'%' || lower(_SEARCH) || N'%'
			) a
		ORDER BY
			CASE WHEN _SORT = 0 THEN (SELECT MAX(REG_DATE) FROM pangya.pangya_guild_bbs_res WHERE BBS_SEQ = a.SEQ AND STATE = 1) ELSE null END DESC,
			CASE WHEN _SORT = 6 THEN a.REG_DATE ELSE a.REG_DATE END DESC,
			a.SEQ DESC
			OFFSET (_PAGE * 14) ROWS
			FETCH NEXT 14 ROWS ONLY;

	END IF; -- CHECK PAGE
END;
$$;


ALTER FUNCTION pangya.procgetlistguildbbs(_page integer, _category integer, _search text, _sort integer) OWNER TO postgres;

--
-- TOC entry 650 (class 1255 OID 23333)
-- Name: procgetlistguildweb(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetlistguildweb(_opt integer) RETURNS TABLE("_GUILD_INTRO_IMG_" character varying, "_GUILD_UID_" bigint, "_GUILD_NAME_" character varying, "_GUILD_MARK_IMG_IDX_" integer, "_GUILD_NEW_MARK_IDX_" integer, "_GUILD_MASTER_" character varying, "_GUILD_SUB_MASTER_" character varying, "_MEMBERS_" bigint, "_GUILD_CONDITION_LEVEL_" smallint, "_GUILD_STATE_" smallint, "_GUILD_FLAG_" smallint, "_GUILD_PERMITION_JOIN_" smallint, "_GUILD_POINT_" bigint, "_GUILD_PANG_" bigint, "_GUILD_INFO_" character varying, "_GUILD_NOTICE_" character varying, "_GUILD_REG_DATE_" character varying, "_GUILD_ACCEPT_DATE_" character varying, "_GUILD_CLOSURE_DATE_" character varying)
    LANGUAGE plpgsql
    AS $$
BEGIN
	
	RETURN QUERY SELECT
		a.GUILD_INTRO_IMG,
		a.GUILD_UID,
		a.GUILD_NAME,
		a.GUILD_MARK_IMG_IDX,
		a.GUILD_NEW_MARK_IDX,
		COALESCE((SELECT NICK FROM pangya.account WHERE UID = a.GUILD_LEADER), '') AS GUILD_MASTER,
		COALESCE((SELECT NICK FROM pangya.account WHERE UID = a.GUILD_SUB_MASTER), '') AS GUILD_SUB_MASTER,
		(SELECT COUNT(GUILD_UID) FROM pangya.pangya_guild_member WHERE GUILD_UID = a.GUILD_UID AND MEMBER_STATE_FLAG < 9) as MEMBERS,
		a.GUILD_CONDITION_LEVEL,
		a.GUILD_STATE,
		a.GUILD_FLAG,
		a.GUILD_PERMITION_JOIN,
		a.GUILD_POINT,
		a.GUILD_PANG,
		a.GUILD_INFO,
		a.GUILD_NOTICE,
		CAST(a.GUILD_REG_DATE AS VARCHAR(19)) AS GUILD_REG_DATE,
		CAST(a.GUILD_ACCEPT_DATE AS VARCHAR(19)) AS GUILD_ACCEPT_DATE,
		CAST(a.GUILD_CLOSURE_DATE AS VARCHAR(19)) AS GUILD_CLOSURE_DATE
	FROM
		pangya.pangya_guild a
	WHERE
		_OPT = 0 OR (a.GUILD_STATE = 0/*pending*/ AND a.GUILD_MARK_IMG_IDX = 0 AND a.GUILD_NEW_MARK_IDX > 0
				 OR (a.GUILD_STATE < 3/*3 pra cima é guild bloqueado ou fechada*/ AND  a.GUILD_NEW_MARK_IDX > 0))
	ORDER BY a.GUILD_REG_DATE;
END;
$$;


ALTER FUNCTION pangya.procgetlistguildweb(_opt integer) OWNER TO postgres;

--
-- TOC entry 651 (class 1255 OID 23334)
-- Name: procgetmacrosuser(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetmacrosuser(_iduser integer) RETURNS TABLE("_MACRO1_" character varying, "_MACRO2_" character varying, "_MACRO3_" character varying, "_MACRO4_" character varying, "_MACRO5_" character varying, "_MACRO6_" character varying, "_MACRO7_" character varying, "_MACRO8_" character varying, "_MACRO9_" character varying)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT 
		 Macro1, 
		 Macro2, 
		 Macro3, 
		 Macro4, 
		 Macro5, 
		 Macro6, 
		 Macro7, 
		 Macro8, 
		 Macro9
	FROM pangya.pangya_user_macro
	WHERE UID = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.procgetmacrosuser(_iduser integer) OWNER TO postgres;

--
-- TOC entry 652 (class 1255 OID 23335)
-- Name: procgetmactable(); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetmactable() RETURNS TABLE(_mac_ character varying, _date_ timestamp without time zone, _index_ bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	
	RETURN QUERY SELECT 
		mac, date, index 
	FROM 
		pangya.pangya_mac_table
	ORDER BY date;
END;
$$;


ALTER FUNCTION pangya.procgetmactable() OWNER TO postgres;

--
-- TOC entry 653 (class 1255 OID 23336)
-- Name: procgetmapstatistics(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetmapstatistics(_iduser integer, _tipo_in integer) RETURNS TABLE("_UID_" integer, _tipo_ smallint, _course_ smallint, _tacada_ integer, _putt_ integer, _hole_ integer, _fairway_ integer, _holein_ integer, _puttin_ integer, _total_score_ integer, _best_score_ smallint, _best_pang_ bigint, _character_typeid_ integer, _event_score_ smallint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	
	IF _tipo_in < 10 THEN	-- Seta para zero por que está pedindo a season, aqui não implementei por que nao tem season anterior e o 5 é season atual
		_tipo_in = 0;
	ELSIF _tipo_in = 10 THEN
		_tipo_in = 51;
	ELSIF _tipo_in = 11 THEN
		_tipo_in = 52;
	END IF;

	RETURN QUERY SELECT 
		UID, 
		tipo, 
		course, 
		tacada, 
		putt, 
		hole, 
		fairway, 
		holein, 
		puttin, 
		total_score, 
		best_score, 
		best_pang, 
		character_typeid, 
		event_score
	FROM pangya.pangya_record
	WHERE 
		pangya_record.UID = _IDUSER AND 
		(pangya_record.tipo = _tipo_in AND pangya_record.course < 22) AND 
		pangya_record.assist = 0
		ORDER BY pangya_record.course;
END;
$$;


ALTER FUNCTION pangya.procgetmapstatistics(_iduser integer, _tipo_in integer) OWNER TO postgres;

--
-- TOC entry 654 (class 1255 OID 23337)
-- Name: procgetmapstatisticsassist(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetmapstatisticsassist(_iduser integer, _tipo_in integer) RETURNS TABLE("_UID_" integer, _tipo_ smallint, _course_ smallint, _tacada_ integer, _putt_ integer, _hole_ integer, _fairway_ integer, _holein_ integer, _puttin_ integer, _total_score_ integer, _best_score_ smallint, _best_pang_ bigint, _character_typeid_ integer, _event_score_ smallint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	
	IF _tipo_in < 10 THEN	-- Seta para zero por que está pedindo a season, aqui não implementei por que nao tem season anterior e o 5 é season atual
		_tipo_in = 0;
	ELSIF _tipo_in = 10 THEN
		_tipo_in = 51;
	ELSIF _tipo_in = 11 THEN
		_tipo_in = 52;
	END IF;

	RETURN QUERY SELECT 
		UID, 
		tipo, 
		course, 
		tacada, 
		putt, 
		hole, 
		fairway, 
		holein, 
		puttin, 
		total_score, 
		best_score, 
		best_pang, 
		character_typeid, 
		event_score
	FROM pangya.pangya_record
	WHERE 
		pangya_record.UID = _IDUSER AND 
		(pangya_record.tipo = _tipo_in AND pangya_record.course < 22) AND 
		pangya_record.assist = 1
		ORDER BY pangya_record.course;
END;
$$;


ALTER FUNCTION pangya.procgetmapstatisticsassist(_iduser integer, _tipo_in integer) OWNER TO postgres;

--
-- TOC entry 655 (class 1255 OID 23338)
-- Name: procgetmapstatisticsassistone(integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetmapstatisticsassistone(_iduser integer, _tipo_in integer, _course_in integer) RETURNS TABLE("_UID_" integer, _tipo_ smallint, _course_ smallint, _tacada_ integer, _putt_ integer, _hole_ integer, _fairway_ integer, _holein_ integer, _puttin_ integer, _total_score_ integer, _best_score_ smallint, _best_pang_ bigint, _character_typeid_ integer, _event_score_ smallint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	
	IF _tipo_in < 10 THEN	-- Seta para zero por que está pedindo a season, aqui não implementei por que nao tem season anterior e o 5 é season atual
		_tipo_in = 0;
	ELSIF _tipo_in = 10 THEN
		_tipo_in = 51;
	ELSIF _tipo_in = 11 THEN
		_tipo_in = 52;
	END IF;

	RETURN QUERY SELECT 
		UID, 
		tipo, 
		course, 
		tacada, 
		putt, 
		hole, 
		fairway, 
		holein, 
		puttin, 
		total_score, 
		best_score, 
		best_pang, 
		character_typeid, 
		event_score
	FROM pangya.pangya_record
	WHERE 
		pangya_record.UID = _IDUSER AND 
		(pangya_record.tipo = _tipo_in AND pangya_record.course < 22) AND 
		pangya_record.assist = 1 AND
		pangya_record.course = _course_in
		ORDER BY pangya_record.course;
END;
$$;


ALTER FUNCTION pangya.procgetmapstatisticsassistone(_iduser integer, _tipo_in integer, _course_in integer) OWNER TO postgres;

--
-- TOC entry 656 (class 1255 OID 23339)
-- Name: procgetmapstatisticsone(integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetmapstatisticsone(_iduser integer, _tipo_in integer, _course_in integer) RETURNS TABLE("_UID_" integer, _tipo_ smallint, _course_ smallint, _tacada_ integer, _putt_ integer, _hole_ integer, _fairway_ integer, _holein_ integer, _puttin_ integer, _total_score_ integer, _best_score_ smallint, _best_pang_ bigint, _character_typeid_ integer, _event_score_ smallint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	
	IF _tipo_in < 10 THEN	-- Seta para zero por que está pedindo a season, aqui não implementei por que nao tem season anterior e o 5 é season atual
		_tipo_in = 0;
	ELSIF _tipo_in = 10 THEN
		_tipo_in = 51;
	ELSIF _tipo_in = 11 THEN
		_tipo_in = 52;
	END IF;

	RETURN QUERY SELECT 
		UID, 
		tipo, 
		course, 
		tacada, 
		putt, 
		hole, 
		fairway, 
		holein, 
		puttin, 
		total_score, 
		best_score, 
		best_pang, 
		character_typeid, 
		event_score
	FROM pangya.pangya_record
	WHERE 
		pangya_record.UID = _IDUSER AND 
		(pangya_record.tipo = _tipo_in AND pangya_record.course < 22) AND 
		pangya_record.assist = 0 AND
		pangya_record.course = _course_in
		ORDER BY pangya_record.course;
END;
$$;


ALTER FUNCTION pangya.procgetmapstatisticsone(_iduser integer, _tipo_in integer, _course_in integer) OWNER TO postgres;

--
-- TOC entry 657 (class 1255 OID 23340)
-- Name: procgetmascotinfo(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetmascotinfo(_iduser integer) RETURNS TABLE(_item_id_ bigint, "_UID_" integer, _typeid_ integer, "_mLevel_" smallint, "_mExp_" integer, "_Flag_" smallint, "_Message_" character varying, "_Tipo_" smallint, "_IsCash_" smallint, "_EndDate_" timestamp without time zone)
    LANGUAGE plpgsql
    AS $$
BEGIN
	
	RETURN QUERY SELECT 
         pangya_mascot_info.item_id, 
         pangya_mascot_info.UID, 
         pangya_mascot_info.typeid, 
         pangya_mascot_info.mLevel, 
         pangya_mascot_info.mExp, 
         pangya_mascot_info.Flag, 
         pangya_mascot_info.Message, 
         pangya_mascot_info.Tipo, 
         pangya_mascot_info.IsCash, 
         pangya_mascot_info.EndDate
      FROM pangya.pangya_mascot_info
      WHERE 
         pangya_mascot_info.UID = _IDUSER AND 
         (
         CASE 
            WHEN Tipo = 1 THEN 
               CASE 
                  WHEN (pangya_mascot_info.EndDate >= now()) THEN 1
                  ELSE 0
               END
            ELSE 
               1
         END <> 0) AND 
         pangya_mascot_info.Valid = 1;
END;
$$;


ALTER FUNCTION pangya.procgetmascotinfo(_iduser integer) OWNER TO postgres;

--
-- TOC entry 658 (class 1255 OID 23341)
-- Name: procgetmascotinfo_one(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetmascotinfo_one(_iduser integer, _iditem integer) RETURNS TABLE(_item_id_ bigint, "_UID_" integer, _typeid_ integer, "_mLevel_" smallint, "_mExp_" integer, "_Flag_" smallint, "_Message_" character varying, "_Tipo_" smallint, "_IsCash_" smallint, "_EndDate_" timestamp without time zone)
    LANGUAGE plpgsql
    AS $$
BEGIN

	IF _IDITEM < 0 THEN
		SELECT pangya_user_equip.mascot_id INTO _IDITEM
		FROM pangya.pangya_user_equip
		WHERE pangya_user_equip.UID = _IDUSER;
	END IF;
	
	RETURN QUERY SELECT 
         pangya_mascot_info.item_id, 
         pangya_mascot_info.UID, 
         pangya_mascot_info.typeid, 
         pangya_mascot_info.mLevel, 
         pangya_mascot_info.mExp, 
         pangya_mascot_info.Flag, 
         pangya_mascot_info.Message, 
         pangya_mascot_info.Tipo, 
         pangya_mascot_info.IsCash, 
         pangya_mascot_info.EndDate
      FROM pangya.pangya_mascot_info
      WHERE 
         pangya_mascot_info.UID = _IDUSER AND 
		 pangya_mascot_info.item_id = _IDITEM AND 
         (
         CASE 
            WHEN Tipo = 1 THEN 
               CASE 
                  WHEN (pangya_mascot_info.EndDate >= now()) THEN 1
                  ELSE 0
               END
            ELSE 
               1
         END <> 0) AND 
         pangya_mascot_info.Valid = 1;
END;
$$;


ALTER FUNCTION pangya.procgetmascotinfo_one(_iduser integer, _iditem integer) OWNER TO postgres;

--
-- TOC entry 659 (class 1255 OID 23342)
-- Name: procgetmemberinfo(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetmemberinfo(_iduser integer) RETURNS TABLE("_ID_" character varying, "_PASSWORD_" character varying, "_NICK_" character varying)
    LANGUAGE plpgsql
    AS $$
BEGIN

	RETURN QUERY SELECT account.ID, account.PASSWORD, account.NICK
      FROM pangya.account
      WHERE account.UID = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.procgetmemberinfo(_iduser integer) OWNER TO postgres;

--
-- TOC entry 660 (class 1255 OID 23343)
-- Name: procgetmemorialnormaliteminfo(); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetmemorialnormaliteminfo() RETURNS TABLE(_typeid_ integer, _tipo_ smallint, _item_typeid_ integer, _item_qntd_ integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT 
		a.typeid
	   ,a.tipo
	   ,CASE WHEN b.typeid IS NULL THEN a.typeid ELSE b.typeid END as item_typeid
	   ,CASE WHEN b.qntd IS NULL THEN a.qntd ELSE b.qntd END as item_qntd 
	FROM pangya.pangya_new_memorial_normal_item a
	LEFT OUTER JOIN
	pangya.pangya_new_memorial_lucky_set b
	ON a.typeid = b.set_id
	WHERE a.active = 1;
END;
$$;


ALTER FUNCTION pangya.procgetmemorialnormaliteminfo() OWNER TO postgres;

--
-- TOC entry 661 (class 1255 OID 23344)
-- Name: procgetmemorialshoprate(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetmemorialshoprate(_coin_typeid integer) RETURNS TABLE(_prob_ integer, _tipo_ integer)
    LANGUAGE plpgsql
    AS $$
BEGIN

	RETURN QUERY SELECT 
         CASE 
            WHEN (b.tipo = 1 AND a.tipo = 1) THEN a.probabilidade + 400
            ELSE a.probabilidade
         END AS prob, a.tipo
      FROM pangya.pangya_memorial_shop_rate  AS a, pangya.pangya_memorial_shop_coin_item  AS b
      WHERE b.typeid = _COIN_TYPEID;
END;
$$;


ALTER FUNCTION pangya.procgetmemorialshoprate(_coin_typeid integer) OWNER TO postgres;

--
-- TOC entry 662 (class 1255 OID 23345)
-- Name: procgetmoedas(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetmoedas(_iduser integer) RETURNS TABLE("_Pang_" numeric, "_Cookie_" numeric)
    LANGUAGE plpgsql
    AS $$
BEGIN

	RETURN QUERY SELECT user_info.Pang, user_info.Cookie
      FROM pangya.user_info
      WHERE user_info.UID = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.procgetmoedas(_iduser integer) OWNER TO postgres;

--
-- TOC entry 663 (class 1255 OID 23346)
-- Name: procgetmsgoff(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetmsgoff(_iduser integer) RETURNS TABLE(_msg_idx_ bigint, _uid_ integer, "_NICK_" character varying, _msg_ character varying, _reg_date_ timestamp without time zone)
    LANGUAGE plpgsql
    AS $$
BEGIN

	RETURN QUERY SELECT 
         a.msg_idx, 
         a.uid_from AS uid, 
         b.NICK, 
         a.msg, 
         a.reg_date
      FROM pangya.pangya_msg_user  AS a, pangya.account  AS b
      WHERE 
         a.uid_from = b.UID AND 
         a.uid = _IDUSER AND 
         a.valid = 1;

      UPDATE pangya.pangya_msg_user
         SET 
            valid = 0
      WHERE pangya_msg_user.uid = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.procgetmsgoff(_iduser integer) OWNER TO postgres;

--
-- TOC entry 664 (class 1255 OID 23347)
-- Name: procgetmyroom_one(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetmyroom_one(_iduser integer, _itemid integer) RETURNS TABLE("_MYROOM_ID_" bigint, "_UID_" integer, "_TYPEID_" integer, "_ROOM_NO_" integer, "_POS_X_" real, "_POS_Y_" real, "_POS_Z_" real, "_POS_R_" real, "_USE_YN_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN

	RETURN QUERY SELECT 
         td_room_data.MYROOM_ID, 
         td_room_data.UID, 
         td_room_data.TYPEID, 
         td_room_data.ROOM_NO, 
         td_room_data.POS_X, 
         td_room_data.POS_Y, 
         td_room_data.POS_Z, 
         td_room_data.POS_R, 
         (CASE 
            WHEN td_room_data.DISPLAY_YN = 'Y' THEN 1
            ELSE 0
         END) AS USE_YN
      FROM pangya.td_room_data
      WHERE td_room_data.UID = _IDUSER AND valid = 1 AND MYROOM_ID = _ITEMID;
END;
$$;


ALTER FUNCTION pangya.procgetmyroom_one(_iduser integer, _itemid integer) OWNER TO postgres;

--
-- TOC entry 666 (class 1255 OID 23348)
-- Name: procgetnewachievement(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetnewachievement(_iduser integer) RETURNS TABLE(_active_ integer, "_TypeID_" integer, "_ID_ACHIEVEMENT_" integer, _status_ integer, _id_ integer, "_Quest_TypeID_" integer, "_Counter_TypeID_" integer, "_Counter_ID_" integer, "_Count_Num_item_" integer, "_Data_Sec_" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN

	RETURN QUERY SELECT
		a.active,
		a.TypeID,
		a.ID_ACHIEVEMENT,
		a.status,
		b.id,
		b.typeid as Quest_TypeID,
		COALESCE(d.TypeID, 0) as Counter_TypeID,
		COALESCE(c.counter_item_id, 0) as Counter_ID,
		COALESCE(d.Count_Num_Item, 0) as Count_Num_item,
		(CASE WHEN b.Date IS NULL THEN 0 ELSE CAST(public."UNIX_TIMESTAMP"(b.Date) as bigint) END) as Data_Sec
	FROM
		pangya.pangya_achievement a
		INNER JOIN
		pangya.pangya_quest b
		ON a.ID_ACHIEVEMENT = b.achievement_id
		LEFT OUTER JOIN
		(
			SELECT qs.id, 
				(CASE WHEN qs.counter_item_id <> 0 THEN qs.counter_item_id ELSE COALESCE(MIN(sub_qs.counter_item_id), 0) END) as counter_item_id
			FROM pangya.pangya_quest qs
			INNER JOIN
			pangya.pangya_achievement a
			ON qs.achievement_id = a.ID_ACHIEVEMENT
			LEFT JOIN
			(
				SELECT b.ID_ACHIEVEMENT, MIN(a.counter_item_id) as counter_item_id
				FROM pangya.pangya_quest a
				INNER JOIN
				pangya.pangya_achievement b
				ON a.achievement_id = b.ID_ACHIEVEMENT
				WHERE a.uid = _IDUSER AND a.counter_item_id <> 0
				GROUP BY b.ID_ACHIEVEMENT
			) sub_qs
			ON a.ID_ACHIEVEMENT = sub_qs.ID_ACHIEVEMENT
			WHERE qs.uid = _IDUSER
			GROUP BY qs.id, qs.counter_item_id
		) c
		ON b.id = c.id
		LEFT JOIN
		pangya.pangya_counter_item d
		ON c.counter_item_id = d.Count_ID
	WHERE a.UID = _IDUSER
	ORDER BY b.id;
END;
$$;


ALTER FUNCTION pangya.procgetnewachievement(_iduser integer) OWNER TO postgres;

--
-- TOC entry 667 (class 1255 OID 23349)
-- Name: procgetnewauthserverkey(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetnewauthserverkey(_server_uid integer) RETURNS TABLE("NEW_AUTH_SEVER" character varying)
    LANGUAGE plpgsql
    AS $$
BEGIN

	IF _SERVER_UID > 0 THEN
	DECLARE AUTH_KEY VARCHAR(16);
	BEGIN

		SELECT upper(md5(random()::text))::varchar(16) INTO AUTH_KEY;

		IF NOT EXISTS(SELECT * FROM pangya.pangya_auth_key WHERE SERVER_UID = _SERVER_UID) THEN
			-- Make New Line
			INSERT INTO pangya.pangya_auth_key(SERVER_UID, KEY, VALID) VALUES(_SERVER_UID, AUTH_KEY, 1);
		ELSE
			UPDATE pangya.pangya_auth_key SET KEY = AUTH_KEY, valid = 1
			WHERE SERVER_UID = _SERVER_UID;
		END IF;

		RETURN QUERY SELECT AUTH_KEY AS NEW_AUTH_SERVER;
	END;
	-- Parece que ele aceita não retorna nada quando é tabela
	/*ELSE
		RETURN QUERY SELECT NULL::varchar(16) AS NEW_AUTH_SERVER;*/
	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetnewauthserverkey(_server_uid integer) OWNER TO postgres;

--
-- TOC entry 668 (class 1255 OID 23350)
-- Name: procgetnewcounteritem(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetnewcounteritem(_iduser integer) RETURNS TABLE(_active_ integer, "_TypeID_" integer, "_Count_ID_" integer, "_Count_Num_Item_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN

	RETURN QUERY SELECT active, TypeID, Count_ID, Count_Num_Item FROM pangya.pangya_counter_item WHERE UID = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.procgetnewcounteritem(_iduser integer) OWNER TO postgres;

--
-- TOC entry 669 (class 1255 OID 23351)
-- Name: procgetnotice(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetnotice(_id integer) RETURNS TABLE(_message_ character varying)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_NOTICE_ID INT = 0;
	_REFRESH_TIME INT = 0;
BEGIN

	--Get Notice
    _NOTICE_ID = (SELECT arg1 FROM pangya.pangya_command WHERE /*command_id = 0 AND*/ idx = _ID);
	
	UPDATE pangya.pangya_notice_list SET replayCount = replayCount - 1 WHERE notice_id = _NOTICE_ID;
	
	IF (SELECT replayCount FROM pangya.pangya_notice_list WHERE notice_id = _NOTICE_ID) <= 0 THEN
		UPDATE pangya.pangya_command SET valid = 0 WHERE /*command_id = 0 AND*/ idx = _ID;
	ELSE
		_REFRESH_TIME = (SELECT refreshTime FROM pangya.pangya_notice_list WHERE notice_id = _NOTICE_ID);
        
		UPDATE pangya.pangya_command SET reserveDate = (now() + ('1 minute'::interval * _REFRESH_TIME)) WHERE /*command_id = 0 AND*/ idx = _ID;
    END IF;
	
	RETURN QUERY SELECT message FROM pangya.pangya_notice_list WHERE notice_id = _NOTICE_ID;
END;
$$;


ALTER FUNCTION pangya.procgetnotice(_id integer) OWNER TO postgres;

--
-- TOC entry 670 (class 1255 OID 23352)
-- Name: procgetplayerasastate(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetplayerasastate(_iduser integer) RETURNS TABLE("__ASA_STATE_" double precision, "__ANGEL_" integer, "_Sex_" smallint, "_LEVEL_" smallint, "_GAME_COUNT_" bigint)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_ANGEL int = 0;
	_ASA int = 0;
	_ASA_STATE float = 0.0;
BEGIN

	_ASA_STATE = 
         (
            SELECT CASE WHEN user_info.Quitado > 0 THEN user_info.Quitado * 1.0 / user_info.Jogado * 100 ELSE 0.0 END as rate
            FROM pangya.user_info
            WHERE user_info.UID = _IDUSER
         );
		 
	IF _ASA_STATE < 3.0 THEN

		_ASA = 
		(
			SELECT a.EQUIP_TYPE
			FROM pangya.td_char_equip_s4  AS a, pangya.pangya_user_equip  AS b
			WHERE 
				a.UID = _IDUSER AND 
				a.CHAR_ITEMID = b.character_id AND 
				a.EQUIP_TYPE IN ( 
								134309888, 
								134580224, 
								134842368, 
								135120896, 
								135366656, 
								135661568, 
								135858176, 
								136194048, 
								136398848, 
								136660992, 
								137185294, 
								137447424, 
								138004480 )
		);

		IF _ASA <> 0 THEN
			_ANGEL = 1;
		END IF;

	END IF;
	
	RETURN QUERY SELECT 
		_ASA_STATE, 
		_ANGEL, 
		a.Sex, 
		b.level AS LEVEL, 
		b.Jogado AS GAME_COUNT
	FROM pangya.account  AS a, pangya.user_info  AS b
	WHERE a.UID = _IDUSER AND a.UID = b.UID;
END;
$$;


ALTER FUNCTION pangya.procgetplayerasastate(_iduser integer) OWNER TO postgres;

--
-- TOC entry 671 (class 1255 OID 23353)
-- Name: procgetplayercanwingachajpitens(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetplayercanwingachajpitens(_uid integer) RETURNS TABLE(_index_ bigint, _gacha_num_ integer, _rarity_type_ smallint, _typeid_1_ integer, _qnty_1_ bigint, _name_1_ character varying, _char_type_1_ smallint, _typeid_2_ integer, _qnty_2_ bigint, _name_2_ character varying, _char_type_2_ smallint)
    LANGUAGE plpgsql
    AS $$
BEGIN

	IF _UID > 0 THEN
	DECLARE
		_GACHA_NUM INT = COALESCE((SELECT gacha_num FROM pangya.pangya_gacha_jp_rate), 1);
	BEGIN

		RETURN QUERY SELECT
			a.index,
			a.gacha_num,
			a.rarity_type,
			a.typeid_1,
			a.qnty_1,
			(SELECT name FROM pangya.pangya_gacha_jp_all_item_list WHERE typeid = a.typeid_1) AS name_1,
			(SELECT char_type FROM pangya.pangya_gacha_jp_all_item_list WHERE typeid = a.typeid_1) AS char_type_1,
			a.typeid_2,
			a.qnty_2,
			(SELECT name FROM pangya.pangya_gacha_jp_all_item_list WHERE typeid = a.typeid_2) AS name_2,
			(SELECT char_type FROM pangya.pangya_gacha_jp_all_item_list WHERE typeid = a.typeid_2) AS char_type_2
		FROM
			pangya.pangya_gacha_jp_item_list a
		WHERE
			a.active = 1 AND (a.gacha_num = _GACHA_NUM OR a.gacha_num = -1)
			AND (a.rarity_type != 0 OR a.typeid_1 NOT IN(SELECT typeid FROM pangya.pangya_gacha_jp_player_win WHERE UID = _UID AND rarity_type = 0));
	
	END;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetplayercanwingachajpitens(_uid integer) OWNER TO postgres;

--
-- TOC entry 665 (class 1255 OID 23354)
-- Name: procgetplayergachasysteminfo(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetplayergachasysteminfo(_uid integer) RETURNS TABLE("_UID_" bigint, "_ID_" character varying, "_NICKNAME_" character varying, "_LEVEL_" smallint, _capability_ integer, "_IDState_" bigint, "_TICKET_" integer, "_TICKET_ID_" bigint, "_TICKET_SUB_" integer, "_TICKET_SUB_ID_" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN

	IF _UID > 0 THEN
		RETURN QUERY SELECT 
			a.UID,
			a.ID,
			a.NICK AS NICKNAME,
			b.LEVEL,
			a.capability,
			a.IDState,
			COALESCE((SELECT c0 FROM pangya.pangya_item_warehouse WHERE UID = a.UID AND valid = 1 AND typeid = x'1A000080'::int LIMIT 1), 0) AS TICKET,
			COALESCE((SELECT item_id FROM pangya.pangya_item_warehouse WHERE UID = a.UID AND valid = 1 AND typeid = x'1A000080'::int LIMIT 1), -1) AS TICKET_ID,
			COALESCE((SELECT c0 FROM pangya.pangya_item_warehouse WHERE UID = a.UID AND valid = 1 AND typeid = x'1A000083'::int LIMIT 1), 0) AS TICKET_SUB,
			COALESCE((SELECT item_id FROM pangya.pangya_item_warehouse WHERE UID = a.UID AND valid = 1 AND typeid = x'1A000083'::int LIMIT 1), -1) AS TICKET_SUB_ID
		FROM 
			pangya.account a
			INNER JOIN
			pangya.user_info b
			ON a.UID = b.UID
		WHERE
			a.UID = _UID;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetplayergachasysteminfo(_uid integer) OWNER TO postgres;

--
-- TOC entry 672 (class 1255 OID 23355)
-- Name: procgetplayerguildatividade(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetplayerguildatividade(_iduser integer) RETURNS TABLE("_GUILD_NAME_" character varying, "_FLAG_" integer, "_REG_DATE_" character varying)
    LANGUAGE plpgsql
    AS $$
BEGIN

	IF _IDUSER > 0 THEN
		
		RETURN QUERY SELECT
			COALESCE(c.GUILD_NAME, 'Unknown') AS GUILD_NAME,
			b.FLAG,
			CAST(b.REG_DATE AS VARCHAR(10)) AS REG_DATE
		FROM
			pangya.account a
			INNER JOIN
			pangya.pangya_guild_atividade_player b
			ON a.UID = b.UID
			LEFT OUTER JOIN
			pangya.pangya_guild c
			ON c.Guild_UID = b.GUILD_UID
		WHERE a.UID = _IDUSER
		ORDER BY b.REG_DATE DESC
		LIMIT 8;

	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetplayerguildatividade(_iduser integer) OWNER TO postgres;

--
-- TOC entry 673 (class 1255 OID 23356)
-- Name: procgetplayerguildinfo(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetplayerguildinfo(_uid integer) RETURNS TABLE("_UID_" bigint, "_ID_" character varying, "_NICKNAME_" character varying, "_LEVEL_" smallint, _capability_ integer, "_IDState_" bigint, "_GUILD_UID_" bigint, "_MEMBER_STATE_FLAG_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN

	IF _UID > 0 THEN

		RETURN QUERY SELECT 
			a.UID,
			a.ID,
			a.NICK AS NICKNAME,
			b.LEVEL,
			a.capability,
			a.IDState,
			COALESCE(d.Guild_UID, 0) AS GUILD_UID,
			COALESCE(c.MEMBER_STATE_FLAG, -1) AS MEMBER_STATE_FLAG
		FROM 
			pangya.account a
			INNER JOIN
			pangya.user_info b
			ON a.UID = b.UID
			LEFT OUTER JOIN
			pangya.pangya_guild_member c
			ON a.Guild_UID = c.GUILD_UID AND a.UID = c.MEMBER_UID
			LEFT OUTER JOIN
			pangya.pangya_guild d
			ON d.GUILD_UID = a.Guild_UID AND (d.GUILD_STATE NOT IN(4, 5) OR d.GUILD_CLOSURE_DATE IS NULL OR now() < d.GUILD_CLOSURE_DATE)
		WHERE
			a.UID = _UID;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetplayerguildinfo(_uid integer) OWNER TO postgres;

--
-- TOC entry 674 (class 1255 OID 23357)
-- Name: procgetplayerinfogame(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetplayerinfogame(_iduser integer) RETURNS TABLE(_uid_ bigint, _id_ character varying, _nick_ character varying, _password_ character varying, _capability_ integer, _level_ smallint, "_IDState_" bigint, "_BlockTime_" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT a.uid, a.id, a.nick, a.password, a.capability, b.level, a.IDState, COALESCE(public."UNIX_TIMESTAMP"(((a.BlockTime * '1 minute'::interval) + a.BlockRegDate) - now()), -1 ) as BlockTime
	FROM pangya.account a, pangya.user_info b WHERE a.uid = b.uid AND a.uid = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.procgetplayerinfogame(_iduser integer) OWNER TO postgres;

--
-- TOC entry 675 (class 1255 OID 23358)
-- Name: procgetplayerinfologin(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetplayerinfologin(_iduser integer) RETURNS TABLE(_uid bigint, _id character varying, _nick character varying, _password character varying, _capability integer, _level smallint, "_IDState" bigint, "_BlockTime" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT a.uid, a.id, a.nick, a.password, a.capability, b.level, a.IDState, COALESCE(public."UNIX_TIMESTAMP"(((a.BlockTime * '1 minute'::interval) + a.BlockRegDate) - now()), -1) as BlockTime 
		FROM pangya.account a, pangya.user_info b 
		WHERE a.uid = b.uid AND a.uid = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.procgetplayerinfologin(_iduser integer) OWNER TO postgres;

--
-- TOC entry 676 (class 1255 OID 23359)
-- Name: procgetplayerinfomessage(bigint); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetplayerinfomessage(_uid bigint) RETURNS TABLE("_UID_" bigint, _id_ character varying, _nick_ character varying, _capability_ integer, _guild_uid_ bigint, _guild_name_ character varying, _sex_ smallint, _level_ smallint, _game_server_id_ character varying, "_IDState_" bigint, "_BlockTime_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT 
		a.UID,
		a.id,
		a.nick,
		a.capability,
		COALESCE(c.guild_uid, 0) as guild_uid,
		COALESCE(c.GUILD_NAME, '') as guild_name,
		a.sex,
		b.level,
		a.game_server_id,
		a.IDState,
		COALESCE(DATEDIFF('second', now()::timestamp, (a.BlockRegDate + ('1 minute'::interval * a.BlockTime))), -1)::int as BlockTime
	FROM
		pangya.account a
		INNER JOIN
		pangya.user_info b
		ON a.UID = b.UID
		LEFT OUTER JOIN
		(
			SELECT
				a.GUILD_UID,
				a.GUILD_NAME,
				b.MEMBER_UID
			FROM
				pangya.pangya_guild a
				INNER JOIN
				pangya.pangya_guild_member b
				ON a.GUILD_UID = b.GUILD_UID
			WHERE
				b.MEMBER_STATE_FLAG < 9
				AND (a.GUILD_STATE NOT IN(4, 5) OR a.GUILD_CLOSURE_DATE IS NULL OR now() < a.GUILD_CLOSURE_DATE)
		) c
		ON c.GUILD_UID = a.GUILD_UID AND c.MEMBER_UID = a.UID
	WHERE a.UID = _UID;
END;
$$;


ALTER FUNCTION pangya.procgetplayerinfomessage(_uid bigint) OWNER TO postgres;

--
-- TOC entry 677 (class 1255 OID 23360)
-- Name: procgetplayerinforank(bigint); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetplayerinforank(_uid bigint) RETURNS TABLE(_uid_ bigint, _id_ character varying, _nick_ character varying, _capability_ integer, _game_server_id_ character varying, _level_ smallint, "_IDState_" bigint, "_BlockTime_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT a.uid, a.id, a.nick, a.capability, a.game_server_id, b.level, a.IDState, COALESCE(DATEDIFF('second', now()::timestamp, (a.BlockRegDate + ('1 minute'::interval * a.BlockTime))), -1)::int as BlockTime
	FROM pangya.account a, pangya.user_info b WHERE a.uid = b.uid AND a.uid = _UID;
END;
$$;


ALTER FUNCTION pangya.procgetplayerinforank(_uid bigint) OWNER TO postgres;

--
-- TOC entry 678 (class 1255 OID 23361)
-- Name: procgetplayerlogindados(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetplayerlogindados(_iduser integer) RETURNS TABLE(_cap_ integer, "_NICK_" character varying, "_IDState_" bigint, "_BlockTime_" bigint, _level_ smallint, _ip_ character varying)
    LANGUAGE plpgsql
    AS $$
BEGIN

	RETURN QUERY SELECT 
         a.cap, 
         a.NICK, 
         a.IDState, 
         a.BlockTime, 
         b.level, 
         COALESCE(c.ip, '') AS ip
      FROM 
         (
               SELECT uid, account.capability AS cap, account.NICK, account.IDState, COALESCE(public."UNIX_TIMESTAMP"((BlockRegDate + ('1 minute'::interval * BlockTime))), -1) AS BlockTime
               FROM pangya.account
               WHERE account.UID = _IDUSER
            )  AS a 
			INNER JOIN
            (
               SELECT uid, user_info.level
               FROM pangya.user_info
               WHERE user_info.UID = _IDUSER
            )  AS b
			ON a.uid = b.uid
            LEFT OUTER JOIN 
            (
               SELECT pangya_player_ip.uid, pangya_player_ip.ip
               FROM pangya.pangya_player_ip
               WHERE pangya_player_ip.uid = _IDUSER
            )  AS c 
            ON c.uid = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.procgetplayerlogindados(_iduser integer) OWNER TO postgres;

--
-- TOC entry 679 (class 1255 OID 23362)
-- Name: procgetplayerluciaattendanceinfo(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetplayerluciaattendanceinfo(_uid integer) RETURNS TABLE("_UID_" bigint, "_ID_" character varying, "_NICKNAME_" character varying, "_IDState_" bigint, _count_day_ bigint, _last_day_attendance_ timestamp without time zone, _last_day_get_item_ timestamp without time zone, _try_hacking_count_ integer, _block_type_ smallint, _block_end_date_ timestamp without time zone)
    LANGUAGE plpgsql
    AS $$
BEGIN

	IF _UID > 0 THEN
		
		-- Verifica se o player já está cadastrado no sistema da lucia attendance, 
		-- se não tiver cria o cadastro ele liberado o ultimo dia
		IF (SELECT UID FROM pangya.pangya_lucia_attendance WHERE UID = _UID) IS NULL THEN
			INSERT INTO pangya.pangya_lucia_attendance(UID, count_day) VALUES(_UID, 6);
		END IF;

		RETURN QUERY SELECT
			a.UID,
			a.ID,
			a.NICK as NICKNAME,
			a.IDState,
			b.count_day,
			b.last_day_attendance,
			b.last_day_get_item,
			b.try_hacking_count,
			b.block_type,
			b.block_end_date
		FROM
			pangya.account a
			INNER JOIN
			pangya.pangya_lucia_attendance b
			ON a.UID = b.UID
		WHERE a.UID = _UID;

	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetplayerluciaattendanceinfo(_uid integer) OWNER TO postgres;

--
-- TOC entry 681 (class 1255 OID 23363)
-- Name: procgetplayermotionitem(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetplayermotionitem(_iduser integer) RETURNS TABLE("_MOTION_ITEM_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_MOTION_ITEM int = 0;
BEGIN

	_MOTION_ITEM = 
		(
			SELECT a.EQUIP_TYPE
            FROM pangya.td_char_equip_s4  AS a, pangya.pangya_user_equip  AS b
            WHERE 
               a.UID = _IDUSER AND 
               a.CHAR_ITEMID = b.character_id AND 
               a.EQUIP_TYPE IN ( 
               134375424, 
               134375425, 
               134375426, 
               134629376, 
               134629377, 
               134629378, 
               134629379, 
               134883328, 
               134883329, 
               134883330, 
               135153664, 
               135153665, 
               135153666, 
               135407616, 
               135407617, 
               135407618, 
               135718913, 
               135718914, 
               135718915, 
               135718917, 
               135940096, 
               135940097, 
               136226816, 
               136480768, 
               136480769, 
               136480770, 
               136480771, 
               136742912, 
               136996864, 
               137250816, 
               137250817, 
               137496576, 
               137496577, 
               137496578, 
               138037248, 
               138037249, 
               138037250) LIMIT 1
		);
		
		_MOTION_ITEM = 
         (CASE 
            WHEN _MOTION_ITEM IS NULL THEN 0
            ELSE 1
         END);
		 
		RETURN QUERY SELECT _MOTION_ITEM;
END;
$$;


ALTER FUNCTION pangya.procgetplayermotionitem(_iduser integer) OWNER TO postgres;

--
-- TOC entry 682 (class 1255 OID 23364)
-- Name: procgetpremiumticket(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetpremiumticket(_iduser integer) RETURNS TABLE(_item_id_ bigint, _typeid_ integer, _unix_end_date_ bigint, _unix_sec_date_ integer)
    LANGUAGE plpgsql
    AS $$
BEGIN

	RETURN QUERY SELECT item_id, typeid, public."UNIX_TIMESTAMP"(ENDDATE) as unix_end_date, DATEDIFF('second', CURRENT_TIMESTAMP::timestamp, ENDDATE)::int as unix_sec_date FROM pangya.pangya_item_warehouse
	WHERE (typeid = x'1A100002'::int OR typeid = x'1A100003'::int) AND valid = 1 AND APPLYTIME != ENDDATE AND uid = _IDUSER LIMIT 1;
END;
$$;


ALTER FUNCTION pangya.procgetpremiumticket(_iduser integer) OWNER TO postgres;

--
-- TOC entry 683 (class 1255 OID 23365)
-- Name: procgetquestsclear(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetquestsclear(_iduser integer) RETURNS TABLE("_TypeID_" integer, _quest_typeid_ integer)
    LANGUAGE plpgsql
    AS $$
BEGIN

	RETURN QUERY SELECT a.TypeID AS achievement_typeid, b.TypeID_ACHIEVE AS quest_typeid
      FROM pangya.achievement_tipo  AS a, pangya.achievement_quest  AS b
      WHERE 
         a.UID = b.UID AND 
         a.ID_ACHIEVEMENT = b.ID_ACHIEVEMENT AND 
         b.IDX IN 
         (
            SELECT pangya_quest_clear.quest_id
            FROM pangya.pangya_quest_clear
            WHERE pangya_quest_clear.uid = _IDUSER AND pangya_quest_clear.option = 1
         );
END;
$$;


ALTER FUNCTION pangya.procgetquestsclear(_iduser integer) OWNER TO postgres;

--
-- TOC entry 684 (class 1255 OID 23366)
-- Name: procgetrankregistrycharacterinfo(); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetrankregistrycharacterinfo() RETURNS TABLE("_UID_" integer, "_ID_" character varying, "_NICK_" character varying, _level_ integer, _item_id_ integer, _typeid_ integer, "_ITEMID_PARTS_1_" integer, "_ITEMID_PARTS_2_" integer, "_ITEMID_PARTS_3_" integer, "_ITEMID_PARTS_4_" integer, "_ITEMID_PARTS_5_" integer, "_ITEMID_PARTS_6_" integer, "_ITEMID_PARTS_7_" integer, "_ITEMID_PARTS_8_" integer, "_ITEMID_PARTS_9_" integer, "_ITEMID_PARTS_10_" integer, "_ITEMID_PARTS_11_" integer, "_ITEMID_PARTS_12_" integer, "_ITEMID_PARTS_13_" integer, "_ITEMID_PARTS_14_" integer, "_ITEMID_PARTS_15_" integer, "_ITEMID_PARTS_16_" integer, "_ITEMID_PARTS_17_" integer, "_ITEMID_PARTS_18_" integer, "_ITEMID_PARTS_19_" integer, "_ITEMID_PARTS_20_" integer, "_ITEMID_PARTS_21_" integer, "_ITEMID_PARTS_22_" integer, "_ITEMID_PARTS_23_" integer, "_ITEMID_PARTS_24_" integer, _parts_1_ integer, _parts_2_ integer, _parts_3_ integer, _parts_4_ integer, _parts_5_ integer, _parts_6_ integer, _parts_7_ integer, _parts_8_ integer, _parts_9_ integer, _parts_10_ integer, _parts_11_ integer, _parts_12_ integer, _parts_13_ integer, _parts_14_ integer, _parts_15_ integer, _parts_16_ integer, _parts_17_ integer, _parts_18_ integer, _parts_19_ integer, _parts_20_ integer, _parts_21_ integer, _parts_22_ integer, _parts_23_ integer, _parts_24_ integer, _default_hair_ smallint, _default_shirts_ smallint, _gift_flag_ smallint, "_PCL0_" smallint, "_PCL1_" smallint, "_PCL2_" smallint, "_PCL3_" smallint, "_PCL4_" smallint, "_Purchase_" smallint, _auxparts_1_ integer, _auxparts_2_ integer, _auxparts_3_ integer, _auxparts_4_ integer, _auxparts_5_ integer, "_CutIn_1_" integer, "_CutIn_2_" integer, "_CutIn_3_" integer, "_CutIn_4_" integer, "_Mastery_" integer, "_CARD_CHARACTER_1_" integer, "_CARD_CHARACTER_2_" integer, "_CARD_CHARACTER_3_" integer, "_CARD_CHARACTER_4_" integer, "_CARD_CADDIE_1_" integer, "_CARD_CADDIE_2_" integer, "_CARD_CADDIE_3_" integer, "_CARD_CADDIE_4_" integer, "_CARD_NPC_1_" integer, "_CARD_NPC_2_" integer, "_CARD_NPC_3_" integer, "_CARD_NPC_4_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT
		b.UID,
		a.ID,
		a.NICK,
		COALESCE((SELECT valor FROM pangya.pangya_rank_atual WHERE tipo_rank = 2 AND tipo_rank_seq = 3 AND UID = a.UID LIMIT 1), 0) AS level,
		b.item_id, 
        b.typeid, 
        b.ITEMID_PARTS_1, 
        b.ITEMID_PARTS_2, 
        b.ITEMID_PARTS_3, 
        b.ITEMID_PARTS_4, 
        b.ITEMID_PARTS_5, 
        b.ITEMID_PARTS_6, 
        b.ITEMID_PARTS_7, 
        b.ITEMID_PARTS_8, 
        b.ITEMID_PARTS_9, 
        b.ITEMID_PARTS_10, 
        b.ITEMID_PARTS_11, 
        b.ITEMID_PARTS_12, 
        b.ITEMID_PARTS_13, 
        b.ITEMID_PARTS_14, 
        b.ITEMID_PARTS_15, 
        b.ITEMID_PARTS_16, 
        b.ITEMID_PARTS_17, 
        b.ITEMID_PARTS_18, 
        b.ITEMID_PARTS_19, 
        b.ITEMID_PARTS_20, 
        b.ITEMID_PARTS_21, 
        b.ITEMID_PARTS_22, 
        b.ITEMID_PARTS_23, 
        b.ITEMID_PARTS_24, 
        b.parts_1, 
        b.parts_2, 
        b.parts_3, 
        b.parts_4, 
        b.parts_5, 
        b.parts_6, 
        b.parts_7, 
        b.parts_8, 
        b.parts_9, 
        b.parts_10, 
        b.parts_11, 
        b.parts_12, 
        b.parts_13, 
        b.parts_14, 
        b.parts_15, 
        b.parts_16, 
        b.parts_17, 
        b.parts_18, 
        b.parts_19, 
        b.parts_20, 
        b.parts_21, 
        b.parts_22, 
        b.parts_23, 
        b.parts_24, 
        b.default_hair, 
        b.default_shirts, 
        b.gift_flag, 
        b.PCL0, 
        b.PCL1, 
        b.PCL2, 
        b.PCL3, 
        b.PCL4, 
        b.Purchase, 
        b.auxparts_1, 
        b.auxparts_2, 
        b.auxparts_3, 
        b.auxparts_4, 
        b.auxparts_5, 
        b.CutIn_1, 
        b.CutIn_2, 
        b.CutIn_3, 
        b.CutIn_4, 
        b.Mastery, 
        b.CARD_CHARACTER_1, 
        b.CARD_CHARACTER_2, 
        b.CARD_CHARACTER_3, 
        b.CARD_CHARACTER_4, 
        b.CARD_CADDIE_1, 
        b.CARD_CADDIE_2, 
        b.CARD_CADDIE_3, 
        b.CARD_CADDIE_4, 
        b.CARD_NPC_1, 
        b.CARD_NPC_2, 
        b.CARD_NPC_3, 
        b.CARD_NPC_4
	FROM
		pangya.account a
		INNER JOIN
		pangya.pangya_rank_atual_character b
		ON a.UID = b.UID
	ORDER BY a.UID;
END;
$$;


ALTER FUNCTION pangya.procgetrankregistrycharacterinfo() OWNER TO postgres;

--
-- TOC entry 685 (class 1255 OID 23367)
-- Name: procgetrankregistryinfo(); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetrankregistryinfo() RETURNS TABLE("_UID_" integer, "_CURRENT_POSITION_" integer, "_LAST_POSITION_" integer, _valor_ integer, _tipo_rank_ smallint, _tipo_rank_seq_ smallint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT
		a.UID,
		a.position AS CURRENT_POSITION,
		COALESCE((SELECT position FROM pangya.pangya_rank_antes WHERE tipo_rank = a.tipo_rank AND tipo_rank_seq = a.tipo_rank_seq AND UID = a.UID LIMIT 1), 0) as LAST_POSITION,
		a.valor,
		a.tipo_rank,
		a.tipo_rank_seq
	FROM
		pangya.pangya_rank_atual a
	ORDER BY a.tipo_rank, a.tipo_rank_seq;
END;
$$;


ALTER FUNCTION pangya.procgetrankregistryinfo() OWNER TO postgres;

--
-- TOC entry 686 (class 1255 OID 23368)
-- Name: procgetrateconfiginfo(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetrateconfiginfo(_server_uid integer) RETURNS TABLE("_GrandZodiacEventTime_" smallint, "_ScratchyPorPointRate_" smallint, "_PapelShopRareItemRate_" smallint, "_PapelShopCookieItemRate_" smallint, "_TreasureRate_" smallint, "_PangRate_" smallint, "_ExpRate_" smallint, "_ClubMasteryRate_" smallint, "_ChuvaRate_" smallint, "_MemorialShopRate_" smallint, "_AngelEvent_" smallint, "_GrandPrixEvent_" smallint, "_GoldenTimeEvent_" smallint, "_LoginRewardEvent_" smallint, "_BotGMEvent_" smallint, "_SmartCalculator_" smallint)
    LANGUAGE plpgsql
    AS $$
BEGIN

	IF _SERVER_UID > 0 AND (SELECT UID FROM pangya.pangya_config WHERE UID = _SERVER_UID) IS NOT NULL THEN
	BEGIN
		RETURN QUERY SELECT	GrandZodiacEventTime,
				ScratchyPorPointRate,
				PapelShopRareItemRate,
				PapelShopCookieItemRate,
				TreasureRate,
				PangRate,
				ExpRate,
				ClubMasteryRate,
				ChuvaRate,
				MemorialShopRate,
				AngelEvent,
				GrandPrixEvent,
				GoldenTimeEvent,
				LoginRewardEvent,
				BotGMEvent,
				SmartCalculator
		FROM pangya.pangya_config WHERE UID = _SERVER_UID;
	END;
	ELSE
		RETURN QUERY SELECT	-1::smallint AS ERROR,
				0::smallint,
				0::smallint,
				0::smallint,
				0::smallint,
				0::smallint,
				0::smallint,
				0::smallint,
				0::smallint,
				0::smallint,
				0::smallint,
				0::smallint,
				0::smallint,
				0::smallint,
				0::smallint,
				0::smallint;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetrateconfiginfo(_server_uid integer) OWNER TO postgres;

--
-- TOC entry 688 (class 1255 OID 23369)
-- Name: procgetrecordplayercourse(integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetrecordplayercourse(_iduser integer, _mapa_in integer, _tipo_in integer) RETURNS TABLE(_tipo_ smallint, _course_ smallint, _tacada_ integer, _putt_ integer, _hole_ integer, _fairway_ integer, _holein_ integer, _puttin_ integer, _total_score_ integer, _best_score_ smallint, _best_pang_ bigint, _character_typeid_ integer, _event_score_ smallint)
    LANGUAGE plpgsql
    AS $$
BEGIN

		IF 
         (
            SELECT pangya_record.course
            FROM pangya.pangya_record
            WHERE 
               pangya_record.UID = _IDUSER AND 
               pangya_record.course = _mapa_in AND 
               pangya_record.tipo = _tipo_in LIMIT 1
         ) IS NOT NULL THEN
			 RETURN QUERY SELECT 
				pangya_record.tipo, 
				pangya_record.course, 
				pangya_record.tacada, 
				pangya_record.putt, 
				pangya_record.hole, 
				pangya_record.fairway, 
				pangya_record.holein, 
				pangya_record.puttin, 
				pangya_record.total_score, 
				pangya_record.best_score, 
				pangya_record.best_pang, 
				pangya_record.character_typeid, 
				pangya_record.event_score
			 FROM pangya.pangya_record
			 WHERE 
				pangya_record.UID = _IDUSER AND 
				pangya_record.course = _mapa_in AND 
				pangya_record.tipo = _tipo_in;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetrecordplayercourse(_iduser integer, _mapa_in integer, _tipo_in integer) OWNER TO postgres;

--
-- TOC entry 689 (class 1255 OID 23370)
-- Name: procgetroom(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetroom(_iduser integer) RETURNS TABLE("_MYROOM_ID_" bigint, "_UID_" integer, "_TYPEID_" integer, "_ROOM_NO_" integer, "_POS_X_" real, "_POS_Y_" real, "_POS_Z_" real, "_POS_R_" real, "_USE_YN_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN

	RETURN QUERY SELECT 
         td_room_data.MYROOM_ID, 
         td_room_data.UID, 
         td_room_data.TYPEID, 
         td_room_data.ROOM_NO, 
         td_room_data.POS_X, 
         td_room_data.POS_Y, 
         td_room_data.POS_Z, 
         td_room_data.POS_R, 
         (CASE 
            WHEN td_room_data.DISPLAY_YN = 'Y' THEN 1
            ELSE 0
         END) AS USE_YN
      FROM pangya.td_room_data
      WHERE td_room_data.UID = _IDUSER AND valid = 1;
END;
$$;


ALTER FUNCTION pangya.procgetroom(_iduser integer) OWNER TO postgres;

--
-- TOC entry 690 (class 1255 OID 23371)
-- Name: procgetserverlist(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetserverlist(opt integer) RETURNS TABLE("Name" character varying, "UID" integer, "IP" character varying, "Port" integer, "MaxUser" integer, "CurrUser" integer, propertys integer, "AngelicWingsNum" integer, "EventFlag" smallint, "EventMap" integer, "ImgNo" smallint, "AppRate" smallint, "ScratchRate" smallint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF OPT = 0 THEN -- Game Server
		RETURN QUERY SELECT 
			Name, 
			UID, 
			IP, 
			Port, 
			MaxUser, 
			CurrUser, 
			property, 
			AngelicWingsNum, 
			EventFlag, 
			EventMap, 
			ImgNo, 
			AppRate, 
			ScratchRate
		FROM pangya.pangya_server_list
		WHERE 
			Type = 1 AND 
			UpdateTime > (now() + '-8 second'::interval) AND 
			State = 1;
	ELSIF OPT = 1 THEN -- Message Server
		RETURN QUERY SELECT 
			Name, 
			UID, 
			IP, 
			Port, 
			MaxUser, 
			CurrUser, 
			property, 
			AngelicWingsNum, 
			EventFlag, 
			EventMap, 
			ImgNo, 
			AppRate, 
			ScratchRate
		FROM pangya.pangya_server_list
		WHERE 
			Type = 3 AND 
			UpdateTime > (now() + '-8 second'::interval) AND 
			State = 1;
	ELSIF OPT = 2 THEN -- Login Server 
		RETURN QUERY SELECT 
			Name, 
			UID, 
			IP, 
			Port, 
			MaxUser, 
			CurrUser, 
			property, 
			AngelicWingsNum, 
			EventFlag, 
			EventMap, 
			ImgNo, 
			AppRate, 
			ScratchRate
		FROM pangya.pangya_server_list
		WHERE 
			Type = 0 AND 
			UpdateTime > (now() + '-11 second'::interval) AND 
			State = 1;
	ELSIF OPT = 3 THEN -- Rank Server
		RETURN QUERY SELECT 
			Name, 
			UID, 
			IP, 
			Port, 
			MaxUser, 
			CurrUser, 
			property, 
			AngelicWingsNum, 
			EventFlag, 
			EventMap, 
			ImgNo, 
			AppRate, 
			ScratchRate
		FROM pangya.pangya_server_list
		WHERE 
			Type = 4 AND 
			UpdateTime > (now() + '-11 second'::interval) AND 
			State = 1;
	ELSIF OPT = 4 THEN -- Auth Server
		RETURN QUERY SELECT 
			Name, 
			UID, 
			IP, 
			Port, 
			MaxUser, 
			CurrUser, 
			property, 
			AngelicWingsNum, 
			EventFlag, 
			EventMap, 
			ImgNo, 
			AppRate, 
			ScratchRate
		FROM pangya.pangya_server_list
		WHERE 
			Type = 5 AND 
			UpdateTime > (now() + '-11 second'::interval) AND 
			State = 1;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetserverlist(opt integer) OWNER TO postgres;

--
-- TOC entry 691 (class 1255 OID 23372)
-- Name: procgetshutdownserver(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetshutdownserver(_id integer) RETURNS TABLE(_sec_ integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_SHUTDOWN_ID INT = 0;
	_REFRESH_TIME INT = 0;
BEGIN

	_SHUTDOWN_ID = (SELECT arg1 FROM pangya.pangya_command WHERE command_id = 3 AND idx = _ID);
    
    UPDATE pangya.pangya_shutdown_list SET replayCount = replayCount - 1 WHERE id = _SHUTDOWN_ID;
    
    IF (SELECT replayCount FROM pangya.pangya_shutdown_list WHERE id = _SHUTDOWN_ID) <= 0 THEN
		UPDATE pangya_command SET valid = 0 WHERE command_id = 3 AND idx = _ID;
	ELSE
		_REFRESH_TIME = (SELECT refreshTime FROM pangya.pangya_shutdown_list WHERE id = _SHUTDOWN_ID);
        
		UPDATE pangya.pangya_command SET reserveDate = (now() + ('1 minute'::interval * _REFRESH_TIME)) WHERE command_id = 3 AND idx = _ID;
    END IF;
    
	RETURN QUERY SELECT 
        datediff('second', now()::timestamp, date_shutdown)::int as sec
    FROM pangya.pangya_shutdown_list WHERE id = _SHUTDOWN_ID;
END;
$$;


ALTER FUNCTION pangya.procgetshutdownserver(_id integer) OWNER TO postgres;

--
-- TOC entry 692 (class 1255 OID 23373)
-- Name: procgettempuplevelclubset(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgettempuplevelclubset(_iduser integer) RETURNS TABLE(_item_id_ integer, "_State_" smallint, "_ITEM_TYPEID_USADO_" integer, "_Quantidade_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN

	RETURN QUERY SELECT pangya_last_up_clubset.item_id, pangya_last_up_clubset.State, pangya_last_up_clubset.item_usado AS ITEM_TYPEID_USADO, pangya_last_up_clubset.Quantidade
      FROM pangya.pangya_last_up_clubset
      WHERE pangya_last_up_clubset.UID = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.procgettempuplevelclubset(_iduser integer) OWNER TO postgres;

--
-- TOC entry 693 (class 1255 OID 23374)
-- Name: procgetticker(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetticker(_id integer) RETURNS TABLE(_message_ character varying, _nick_ character varying)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_TICKER_ID INT = 0;
	_REFRESH_TIME INT = 0;
BEGIN

	--#Pega o Ticker do pangya_command e pangya_ticker_list
    _TICKER_ID = (SELECT arg1 FROM pangya.pangya_command WHERE command_id = 1 AND idx = _ID);
    
    UPDATE pangya.pangya_ticker_list SET replayCount = replayCount - 1 WHERE ticker_id = _TICKER_ID;
    
    IF (SELECT replayCount FROM pangya.pangya_ticker_list WHERE ticker_id = _TICKER_ID) <= 0 THEN
		UPDATE pangya.pangya_command SET valid = 0 WHERE command_id = 1 AND idx = _ID;
	ELSE
		_REFRESH_TIME = (SELECT refreshTime FROM pangya.pangya_ticker_list WHERE ticker_id = _TICKER_ID);
        
		UPDATE pangya.pangya_command SET reserveDate = (now() + ('1 minute'::interval * _REFRESH_TIME)) WHERE command_id = 1 AND idx = _ID;
    END IF;
	
	RETURN QUERY SELECT message, nick  FROM pangya.pangya_ticker_list WHERE ticker_id = _TICKER_ID;
END;
$$;


ALTER FUNCTION pangya.procgetticker(_id integer) OWNER TO postgres;

--
-- TOC entry 694 (class 1255 OID 23375)
-- Name: procgetticketreportdados(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetticketreportdados(_ticket_report_id integer) RETURNS TABLE(_idx_ bigint, _tipo_ integer, _trofel_typeid_ integer, _reg_date_ timestamp without time zone, _player_uid_ integer, _player_score_ smallint, _player_medalha_ smallint, _player_trofel_ smallint, _player_pang_ bigint, _player_bonus_pang_ bigint, _player_exp_ integer, _player_mascot_typeid_ integer, _player_state_ smallint, _flag_item_pang_ smallint, _flag_premium_user_ smallint, _level_ smallint, "_ID_" character varying, "_NICK_" character varying, "_Guild_UID_" integer, "_GUILD_MARK_IMG_" character varying, "_GUILD_MARK_IMG_IDX_" integer, _finish_date_ timestamp without time zone)
    LANGUAGE plpgsql
    AS $$
BEGIN

	IF _ticket_report_id > 0 THEN

		RETURN QUERY SELECT 
			A.idx,
			A.tipo, 
			A.trofel_typeid,
			A.reg_date,
			B.player_uid, 
			B.player_score, 
			B.player_medalha, 
			B.player_trofel, 
			B.player_pang, 
			B.player_bonus_pang, 
			B.player_exp, 
			B.player_mascot_typeid, 
			B.player_state, 
			B.flag_item_pang, 
			B.flag_premium_user, 
			D.level, 
			C.ID, 
			C.NICK, 
			C.Guild_UID, 
			COALESCE(E.GUILD_MARK_IMG, 'guild_mark') as GUILD_MARK_IMG,
			COALESCE(E.GUILD_MARK_IMG_IDX, 0) AS GUILD_MARK_IMG_IDX,
			B.finish_date
		FROM 
			pangya.pangya_ticket_report A
			INNER JOIN 
			pangya.pangya_ticket_report_dados B
			ON A.idx = B.report_id
			INNER JOIN
			pangya.account C
			ON B.player_uid = C.UID
			INNER JOIN
			pangya.user_info D
			ON C.UID = D.UID
			LEFT OUTER JOIN
			pangya.pangya_guild E
			ON C.Guild_UID = E.GUILD_UID
		WHERE 
			A.idx = _ticket_report_id;

	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetticketreportdados(_ticket_report_id integer) OWNER TO postgres;

--
-- TOC entry 680 (class 1255 OID 23376)
-- Name: procgettikipoints(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgettikipoints(_iduser integer) RETURNS TABLE("_TIKI_POINTS_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_TIKI_PTS int;
BEGIN

	SELECT pangya_tiki_points.Tiki_Points INTO _TIKI_PTS
	FROM pangya.pangya_tiki_points
	WHERE pangya_tiki_points.UID = _IDUSER;

	IF _TIKI_PTS IS NULL THEN
	BEGIN

		INSERT INTO pangya.pangya_tiki_points(pangya.pangya_tiki_points.UID, pangya.pangya_tiki_points.Tiki_Points, pangya.pangya_tiki_points.REG_DATE, pangya.pangya_tiki_points.MOD_DATE)
		VALUES (_IDUSER, 0, now(), now());

		SELECT pangya_tiki_points.Tiki_Points AS TIKI_POINTS
		FROM pangya.pangya_tiki_points
		WHERE pangya_tiki_points.UID = _IDUSER;

	END;
	ELSE 
		RETURN QUERY SELECT _TIKI_PTS AS TIKI_POINTS;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procgettikipoints(_iduser integer) OWNER TO postgres;

--
-- TOC entry 695 (class 1255 OID 23377)
-- Name: procgettrofel(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgettrofel(_iduser integer, _opt_in integer) RETURNS TABLE("_AMA_6_G_" smallint, "_AMA_6_S_" smallint, "_AMA_6_B_" smallint, "_AMA_5_G_" smallint, "_AMA_5_S_" smallint, "_AMA_5_B_" smallint, "_AMA_4_G_" smallint, "_AMA_4_S_" smallint, "_AMA_4_B_" smallint, "_AMA_3_G_" smallint, "_AMA_3_S_" smallint, "_AMA_3_B_" smallint, "_AMA_2_G_" smallint, "_AMA_2_S_" smallint, "_AMA_2_B_" smallint, "_AMA_1_G_" smallint, "_AMA_1_S_" smallint, "_AMA_1_B_" smallint, "_PRO_1_G_" smallint, "_PRO_1_S_" smallint, "_PRO_1_B_" smallint, "_PRO_2_G_" smallint, "_PRO_2_S_" smallint, "_PRO_2_B_" smallint, "_PRO_3_G_" smallint, "_PRO_3_S_" smallint, "_PRO_3_B_" smallint, "_PRO_4_G_" smallint, "_PRO_4_S_" smallint, "_PRO_4_B_" smallint, "_PRO_5_G_" smallint, "_PRO_5_S_" smallint, "_PRO_5_B_" smallint, "_PRO_6_G_" smallint, "_PRO_6_S_" smallint, "_PRO_6_B_" smallint, "_PRO_7_G_" smallint, "_PRO_7_S_" smallint, "_PRO_7_B_" smallint)
    LANGUAGE plpgsql
    AS $$
BEGIN

	IF _OPT_IN = 5 THEN
         RETURN QUERY SELECT 
            AMA_6_G, 
            AMA_6_S, 
            AMA_6_B, 
            AMA_5_G, 
            AMA_5_S, 
            AMA_5_B, 
            AMA_4_G, 
            AMA_4_S, 
            AMA_4_B, 
            AMA_3_G, 
            AMA_3_S, 
            AMA_3_B, 
            AMA_2_G, 
            AMA_2_S, 
            AMA_2_B, 
            AMA_1_G, 
            AMA_1_S, 
            AMA_1_B, 
            PRO_1_G, 
            PRO_1_S, 
            PRO_1_B, 
            PRO_2_G, 
            PRO_2_S, 
            PRO_2_B, 
            PRO_3_G, 
            PRO_3_S, 
            PRO_3_B, 
            PRO_4_G, 
            PRO_4_S, 
            PRO_4_B, 
            PRO_5_G, 
            PRO_5_S, 
            PRO_5_B, 
            PRO_6_G, 
            PRO_6_S, 
            PRO_6_B, 
            PRO_7_G, 
            PRO_7_S, 
            PRO_7_B
         FROM pangya.trofel_stat
         WHERE trofel_stat.UID = _IDUSER;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procgettrofel(_iduser integer, _opt_in integer) OWNER TO postgres;

--
-- TOC entry 696 (class 1255 OID 23378)
-- Name: procgettrofelgrandprix(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgettrofelgrandprix(_iduser integer, _opt_in integer) RETURNS TABLE(_item_id_ bigint, _typeid_ integer, _qntd_ integer)
    LANGUAGE plpgsql
    AS $$
BEGIN

	IF _OPT_IN = 5 THEN
         RETURN QUERY SELECT pangya_trofel_grandprix.item_id, pangya_trofel_grandprix.typeid, pangya_trofel_grandprix.qntd
         FROM pangya.pangya_trofel_grandprix
         WHERE pangya_trofel_grandprix.UID = _IDUSER
            ORDER BY pangya_trofel_grandprix.typeid;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procgettrofelgrandprix(_iduser integer, _opt_in integer) OWNER TO postgres;

--
-- TOC entry 697 (class 1255 OID 23379)
-- Name: procgettrofelgrandprixinfo(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgettrofelgrandprixinfo(_iduser integer, _opt_in integer) RETURNS TABLE(_item_id_ bigint, _typeid_ integer, _qntd_ integer)
    LANGUAGE plpgsql
    AS $$
BEGIN

	IF _OPT_IN = 5 THEN
         RETURN QUERY SELECT pangya_trofel_grandprix.item_id, pangya_trofel_grandprix.typeid, pangya_trofel_grandprix.qntd
         FROM pangya.pangya_trofel_grandprix
         WHERE pangya_trofel_grandprix.UID = _IDUSER
            ORDER BY pangya_trofel_grandprix.typeid DESC;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procgettrofelgrandprixinfo(_iduser integer, _opt_in integer) OWNER TO postgres;

--
-- TOC entry 698 (class 1255 OID 23380)
-- Name: procgettrofelspecial(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgettrofelspecial(_iduser integer, _opt_in integer) RETURNS TABLE(_item_id_ bigint, _typeid_ integer, _qntd_ integer)
    LANGUAGE plpgsql
    AS $$
BEGIN

	IF _OPT_IN = 5 THEN
         RETURN QUERY SELECT pangya_trofel_especial.item_id, pangya_trofel_especial.typeid, pangya_trofel_especial.qntd
         FROM pangya.pangya_trofel_especial
         WHERE pangya_trofel_especial.UID = _IDUSER
            ORDER BY pangya_trofel_especial.typeid;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procgettrofelspecial(_iduser integer, _opt_in integer) OWNER TO postgres;

--
-- TOC entry 699 (class 1255 OID 23381)
-- Name: procgettrofelspecialinfo(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgettrofelspecialinfo(_iduser integer, _opt_in integer) RETURNS TABLE(_item_id_ bigint, _typeid_ integer, _qntd_ integer)
    LANGUAGE plpgsql
    AS $$
BEGIN

	IF _OPT_IN = 5 THEN
         RETURN QUERY SELECT pangya_trofel_especial.item_id, pangya_trofel_especial.typeid, pangya_trofel_especial.qntd
         FROM pangya.pangya_trofel_especial
         WHERE pangya_trofel_especial.UID = _IDUSER
            ORDER BY pangya_trofel_especial.typeid DESC;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procgettrofelspecialinfo(_iduser integer, _opt_in integer) OWNER TO postgres;

--
-- TOC entry 700 (class 1255 OID 23382)
-- Name: procgetuserinfo(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetuserinfo(_iduser integer) RETURNS TABLE(_id_ character varying, "_UID_" bigint, "_SEX_" smallint, "_DoTutorial_" smallint, "_NICKNAME_" character varying, "_SCHOOL_" integer, "_CAPABILITY_" integer, "_LOGON_" smallint, "_SERVERID_" character varying, "_MANNERFLAG_" smallint, "_TIMEVAL_" integer, "_GUILD_NAME_" character varying, "_GUILD_UID_" bigint, "_GUILD_PANG_" integer, "_GUILD_POINT_" integer, "_GUILD_MARK_IMG_IDX_" integer, "_EVENT_" smallint, "_EVENT2_" integer, "_LIMIT_CNT_" smallint, "_IDSTATE_" bigint, "_DomainID_" integer, "_CHANNEL_FLAG_" smallint, "_CURRENT_CNT_" smallint, "_REMAIN_CNT_" smallint, "_LAST_UPDATE_" timestamp without time zone, "_LEVEL_" smallint, "_GUILD_MARK_IMG_" character varying, "_EMBLERVER_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDUSER > 0 THEN

		RETURN QUERY SELECT 
		A.ID, 
		A.UID, 
		A.Sex, 
		A.doTutorial, 
		A.NICK, 
		A.School, 
		A.capability, 
		A.Logon, 
		A.ServerID, 
		A.MannerFlag, 
		(CASE WHEN A.LastLeaveTime IS NULL THEN 0 ELSE datediff('minute', A.LastLeaveTime::timestamp, now()::timestamp) END)::int AS TIMEVAL, 
		COALESCE(C.GUILD_NAME, '') AS GUILD_NAME,
		COALESCE(C.GUILD_UID, 0) AS GUILD_UID,
		COALESCE(C.GUILD_PANG, 0) AS GUILD_PANG, 
		COALESCE(C.GUILD_POINT, 0) AS GUILD_POINT,
		COALESCE(C.GUILD_MARK_IMG_IDX, 0) AS GUILD_MARK_IMG_IDX, 
		A.Event, 
		A.Event2, 
		D.limit_cnt, 
		A.IDState, 
		(CASE WHEN A.domainid IS NULL THEN 0 ELSE A.domainid END) AS DOMAINID, 
		A.ChannelFlag, 
		D.current_cnt, 
		D.remain_cnt, 
		D.last_update,
		B.level,
		(CASE WHEN C.GUILD_MARK_IMG IS NULL OR C.MEMBER_STATE_FLAG > 3 THEN '' ELSE C.GUILD_MARK_IMG END) AS GUILD_MARK_IMG,
		0 AS EMBLERVER
		FROM 
			pangya.account AS A 
			INNER JOIN
			pangya.pangya_papel_shop_info AS D
			ON A.UID = D.UID
			INNER JOIN
			pangya.user_info AS B 
			ON D.UID = B.UID
			INNER JOIN 
			(
				SELECT 
					E.GUILD_MARK_IMG_IDX,
					E.GUILD_MARK_IMG, 
					E.GUILD_UID, 
					E.GUILD_NAME, 
					E.MEMBER_STATE_FLAG, 
					E.GUILD_PANG, 
					E.GUILD_POINT, 
					D.UID
				FROM 
					pangya.account D 
					LEFT OUTER JOIN 
					(
						SELECT 
							x.GUILD_MARK_IMG_IDX,
							x.GUILD_MARK_IMG, 
							x.GUILD_UID, 
							x.GUILD_NAME, 
							y.MEMBER_STATE_FLAG, 
							y.GUILD_PANG, 
							y.GUILD_POINT
						FROM 
							pangya.pangya_guild AS x, 
							pangya.pangya_guild_member AS y
						WHERE 
							y.MEMBER_UID = _IDUSER
							AND y.MEMBER_STATE_FLAG < 9
							AND (x.GUILD_STATE NOT IN(4, 5) OR x.GUILD_CLOSURE_DATE IS NULL OR now() < x.GUILD_CLOSURE_DATE)
					) AS E 
					ON D.Guild_UID = E.GUILD_UID
			) AS C 
			ON C.UID = _IDUSER
		WHERE 
			A.UID = _IDUSER;
	
	END IF;
END;
$$;


ALTER FUNCTION pangya.procgetuserinfo(_iduser integer) OWNER TO postgres;

--
-- TOC entry 701 (class 1255 OID 23383)
-- Name: procgetwarehouseitem(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetwarehouseitem(_iduser integer) RETURNS TABLE(_item_id_ bigint, "_UID_" integer, _typeid_ integer, "_Ano_" integer, "_C0_" smallint, "_C1_" smallint, "_C2_" smallint, "_C3_" smallint, "_C4_" smallint, "_Purchase_" smallint, "_Gift_flag_" smallint, "_Flag_" integer, "_APPLY_DATE_" bigint, "_END_DATE_" bigint, "_ItemType_" smallint, "_CARD_CHARACTER_1_" integer, "_CARD_CHARACTER_2_" integer, "_CARD_CHARACTER_3_" integer, "_CARD_CHARACTER_4_" integer, "_CARD_CADDIE_1_" integer, "_CARD_CADDIE_2_" integer, "_CARD_CADDIE_3_" integer, "_CARD_CADDIE_4_" integer, "_CARD_NPC_1_" integer, "_CARD_NPC_2_" integer, "_CARD_NPC_3_" integer, "_CARD_NPC_4_" integer, "_ClubSet_WorkShop_Flag_" smallint, "_ClubSet_WorkShop_C0_" smallint, "_ClubSet_WorkShop_C1_" smallint, "_ClubSet_WorkShop_C2_" smallint, "_ClubSet_WorkShop_C3_" smallint, "_ClubSet_WorkShop_C4_" smallint, "_Mastery_Pts_" integer, "_Recovery_Pts_" integer, "_Level_" integer, "_Up_" integer, "_SD_NAME_" character varying, "_SD_IDX_" character varying, "_SD_SEQ_" integer, "_SD_COPIER_NICK_" character varying, "_SD_COPIER_" integer, "_SD_TRADE_" integer, "_SD_FLAG_" integer, "_SD_STATUS_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN

	RETURN QUERY SELECT A.item_id, A.UID, A.typeid, A.Ano, A.C0, A.C1, A.C2, A.C3, A.C4, A.Purchase, A.Gift_flag, A.Flag, A.APPLY_DATE, A.END_DATE
         				,A.ItemType, B.CARD_CHARACTER_1, B.CARD_CHARACTER_2, B.CARD_CHARACTER_3, B.CARD_CHARACTER_4, B.CARD_CADDIE_1
         				,B.CARD_CADDIE_2, B.CARD_CADDIE_3, B.CARD_CADDIE_4, B.CARD_NPC_1, B.CARD_NPC_2, B.CARD_NPC_3, B.CARD_NPC_4
         				,A.ClubSet_WorkShop_Flag
         				,A.ClubSet_WorkShop_C0
         				,A.ClubSet_WorkShop_C1
         				,A.ClubSet_WorkShop_C2
         				,A.ClubSet_WorkShop_C3
         				,A.ClubSet_WorkShop_C4
         				,A.Mastery_Pts
         				,A.Recovery_Pts
         				,A.Level
         				,A.Up
         				,A.SD_NAME 
         				,A.SD_IDX
         				,A.SD_SEQ
         				,A.SD_COPIER_NICK
         				,A.SD_COPIER
         				,A.SD_TRADE
         				,A.SD_FLAG
         				,A.SD_STATUS
         	FROM
         		(
         			SELECT R.*,  CASE WHEN L.UCC_NAME IS NULL THEN '' ELSE L.UCC_NAME END AS SD_NAME
         						     , CASE WHEN L.UCCIDX IS NULL THEN '' ELSE L.UCCIDX END AS SD_IDX
         						     , CASE WHEN L.SEQ IS NULL THEN 0 ELSE L.SEQ END AS SD_SEQ
         						     , CASE WHEN L.COPIER_NICK IS NULL THEN '' ELSE L.COPIER_NICK END AS SD_COPIER_NICK
         						     , CASE WHEN L.COPIER IS NULL THEN 0 ELSE L.COPIER END AS SD_COPIER
         						     , CASE WHEN L.TRADE IS NULL THEN 0 ELSE L.TRADE END AS SD_TRADE
         						     , CASE WHEN L.FLAG IS NULL THEN 0 ELSE L.FLAG END AS SD_FLAG
         						     , CASE WHEN L.STATUS IS NULL THEN 0 ELSE L.STATUS END AS SD_STATUS
         			FROM 
         			(
         				SELECT d.item_id
         				,d.UID
         				,d.typeid
         				, (CASE WHEN DATEDIFF('minute', (CASE WHEN d.EndDate = d.Applytime THEN d.Applytime ELSE CURRENT_TIMESTAMP END)::timestamp, d.EndDate) > 0 THEN DATEDIFF('hour', CURRENT_TIMESTAMP::timestamp, d.ENDDATE) ELSE -1 END)::int AS Ano
         				,/*ISNULL(E.c0,*/ D.c0--) AS C0
         				,/*ISNULL(E.c1,*/ D.c1--) AS C1
         				,/*ISNULL(E.c2,*/ D.c2--) AS C2
         				,/*ISNULL(E.c3,*/ D.c3--) AS C3
         				,/*ISNULL(E.c4,*/ D.c4--) AS C4
         				,(CASE WHEN DATEDIFF('minute', d.applytime, d.ENDDATE) > 0 THEN public."UNIX_TIMESTAMP"(d.applytime) ELSE 0 END) AS APPLY_DATE
         				,(CASE WHEN DATEDIFF('minute', d.applytime, d.ENDDATE) > 0 THEN public."UNIX_TIMESTAMP"(d.ENDDATE) ELSE 0 END) AS END_DATE --DATEDIFF(CURRENT_TIMESTAMP, regdate) as Date_Diff
         				,d.Gift_flag
         				,(CASE WHEN d.ENDDATE <= CURRENT_TIMESTAMP AND d.ApplyTime != d.ENDDATE THEN d.flag + 2 ELSE d.flag END) AS flag
         				,d.Purchase
         				,d.ItemType
         				,d.ClubSet_WorkShop_Flag
         				,d.ClubSet_WorkShop_C0
         				,d.ClubSet_WorkShop_C1
         				,d.ClubSet_WorkShop_C2
         				,d.ClubSet_WorkShop_C3
         				,d.ClubSet_WorkShop_C4
         				,d.Mastery_Pts
         				,d.Recovery_Pts
         				,d.Level
         				,d.Up
         				FROM 
         				(
         					SELECT * FROM pangya.pangya_item_warehouse 
         					WHERE UID = _IDUSER AND valid = 1 AND Gift_Flag = 0
         					AND (flag = 96 OR applytime = enddate OR typeid = x'1A000042'::int/*TICKET REPORT*/ OR enddate > CURRENT_TIMESTAMP)
         				)D
         				--LEFT OUTER JOIN
         				--(
         				--	SELECT * FROM pangya_clubset_enchant
         				--	WHERE UID = @IDUSER
         				--)E
         				--ON D.item_id = E.item_id
         			)R
         			INNER JOIN
         			(
         				SELECT  F.*,  G.UCC_NAME, G.UCCIDX, G.SEQ, G.COPIER, G.COPIER_NICK, G.TRADE, G.FLAG, G.STATUS
         				FROM 
         				(
         					SELECT item_id
         					FROM pangya.pangya_item_warehouse WHERE UID = _IDUSER AND valid = 1 AND Gift_Flag = 0
         				)F
         				LEFT OUTER JOIN
         				(
         					SELECT UCC_NAME, ITEM_ID, UCCIDX, SEQ, COPIER, COPIER_NICK, TRADE, Flag, STATUS
         					FROM pangya.TU_UCC
         				)G
         				ON F.item_id = G.item_id
         				--GROUP BY F.item_id
         			)L
         			ON R.item_id = L.item_id
         			--GROUP BY R.item_id
         		)A
         		INNER JOIN
         		(
         			SELECT 	item_id
         					, 0 AS CARD_CHARACTER_1
         					, 0 AS CARD_CHARACTER_2
         					, 0 AS CARD_CHARACTER_3
         					, 0 AS CARD_CHARACTER_4
         					, 0 AS CARD_CADDIE_1
         					, 0 AS CARD_CADDIE_2
         					, 0 AS CARD_CADDIE_3
         					, 0 AS CARD_CADDIE_4
         					, 0 AS CARD_NPC_1
         					, 0 AS CARD_NPC_2
         					, 0 AS CARD_NPC_3
         					, 0 AS CARD_NPC_4
         			FROM
         			(
         				SELECT item_id
         				FROM pangya.pangya_item_warehouse
         				WHERE UID = _IDUSER AND valid = 1 AND Gift_Flag = 0
         			)X
         			LEFT OUTER JOIN
         			(
         				SELECT TIPO, Slot, card_typeid, parts_id
         				FROM pangya.pangya_card_equip
         				WHERE UID = _IDUSER AND USE_YN = 1
         			)Y
         			ON Y.parts_id = X.item_id
         			GROUP BY item_id
         		) AS B
         	ON A.item_id = B.item_id
         	ORDER BY A.typeid;
END;
$$;


ALTER FUNCTION pangya.procgetwarehouseitem(_iduser integer) OWNER TO postgres;

--
-- TOC entry 702 (class 1255 OID 23385)
-- Name: procgetwarehouseitem_one(bigint, bigint); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgetwarehouseitem_one(_iduser bigint, _iditem bigint) RETURNS TABLE(_item_id_ bigint, "_UID_" integer, _typeid_ integer, "_Ano_" integer, "_C0_" smallint, "_C1_" smallint, "_C2_" smallint, "_C3_" smallint, "_C4_" smallint, "_Purchase_" smallint, "_Gift_flag_" smallint, "_Flag_" integer, "_APPLY_DATE_" bigint, "_END_DATE_" bigint, "_ItemType_" smallint, "_CARD_CHARACTER_1_" integer, "_CARD_CHARACTER_2_" integer, "_CARD_CHARACTER_3_" integer, "_CARD_CHARACTER_4_" integer, "_CARD_CADDIE_1_" integer, "_CARD_CADDIE_2_" integer, "_CARD_CADDIE_3_" integer, "_CARD_CADDIE_4_" integer, "_CARD_NPC_1_" integer, "_CARD_NPC_2_" integer, "_CARD_NPC_3_" integer, "_CARD_NPC_4_" integer, "_ClubSet_WorkShop_Flag_" smallint, "_ClubSet_WorkShop_C0_" smallint, "_ClubSet_WorkShop_C1_" smallint, "_ClubSet_WorkShop_C2_" smallint, "_ClubSet_WorkShop_C3_" smallint, "_ClubSet_WorkShop_C4_" smallint, "_Mastery_Pts_" integer, "_Recovery_Pts_" integer, "_Level_" integer, "_Up_" integer, "_SD_NAME_" character varying, "_SD_IDX_" character varying, "_SD_SEQ_" integer, "_SD_COPIER_NICK_" character varying, "_SD_COPIER_" integer, "_SD_TRADE_" integer, "_SD_FLAG_" integer, "_SD_STATUS_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN

	IF _IDITEM < 0 THEN
         SELECT pangya_user_equip.club_id INTO _IDITEM
         FROM pangya.pangya_user_equip
         WHERE pangya_user_equip.UID = _IDUSER;
	END IF;

      RETURN QUERY SELECT 
         A.item_id, 
         A.UID, 
         A.typeid, 
         A.Ano, 
         A.C0, 
         A.C1, 
         A.C2, 
         A.C3, 
         A.C4, 
         A.Purchase, 
         A.Gift_flag, 
         A.Flag, 
         A.APPLY_DATE, 
         A.END_DATE, 
         A.ItemType, 
         B.CARD_CHARACTER_1, 
         B.CARD_CHARACTER_2, 
         B.CARD_CHARACTER_3, 
         B.CARD_CHARACTER_4, 
         B.CARD_CADDIE_1, 
         B.CARD_CADDIE_2, 
         B.CARD_CADDIE_3, 
         B.CARD_CADDIE_4, 
         B.CARD_NPC_1, 
         B.CARD_NPC_2, 
         B.CARD_NPC_3, 
         B.CARD_NPC_4, 
         A.ClubSet_WorkShop_Flag, 
         A.ClubSet_WorkShop_C0, 
         A.ClubSet_WorkShop_C1, 
         A.ClubSet_WorkShop_C2, 
         A.ClubSet_WorkShop_C3, 
         A.ClubSet_WorkShop_C4, 
         A.Mastery_Pts, 
         A.Recovery_Pts, 
         A.Level, 
         A.Up, 
         A.SD_NAME, 
         A.SD_IDX, 
         A.SD_SEQ, 
         A.SD_COPIER_NICK, 
         A.SD_COPIER, 
         A.SD_TRADE, 
         A.SD_FLAG, 
         A.SD_STATUS
      FROM 
         (

            SELECT 
               R.item_id, 
               R.UID, 
               R.typeid, 
               R.Ano, 
               R.C0, 
               R.C1, 
               R.C2, 
               R.C3, 
               R.C4, 
               R.APPLY_DATE, 
               R.END_DATE, 
               R.Gift_flag, 
               R.flag, 
               R.Purchase, 
               R.ItemType, 
               R.ClubSet_WorkShop_Flag, 
               R.ClubSet_WorkShop_C0, 
               R.ClubSet_WorkShop_C1, 
               R.ClubSet_WorkShop_C2, 
               R.ClubSet_WorkShop_C3, 
               R.ClubSet_WorkShop_C4, 
               R.Mastery_Pts, 
               R.Recovery_Pts, 
               R.Level, 
               R.Up, 
               CASE 
                  WHEN L.UCC_NAME IS NULL THEN ''
                  ELSE L.UCC_NAME
               END AS SD_NAME, 
                CASE 
                  WHEN L.UCCIDX IS NULL THEN ''
                  ELSE L.UCCIDX
               END AS SD_IDX, 
                CASE 
                  WHEN L.SEQ IS NULL THEN 0
                  ELSE L.SEQ
               END AS SD_SEQ, 
                CASE 
                  WHEN L.COPIER_NICK IS NULL THEN ''
                  ELSE L.COPIER_NICK
               END AS SD_COPIER_NICK, 
                CASE 
                  WHEN L.COPIER IS NULL THEN 0
                  ELSE L.COPIER
               END AS SD_COPIER, 
                CASE 
                  WHEN L.TRADE IS NULL THEN 0
                  ELSE L.TRADE
               END AS SD_TRADE, 
                CASE 
                  WHEN L.Flag IS NULL THEN 0
                  ELSE L.Flag
               END AS SD_FLAG, 
                CASE 
                  WHEN L.STATUS IS NULL THEN 0
                  ELSE L.STATUS
               END AS SD_STATUS
            FROM 
               (
                  
                  SELECT 
                     D.item_id, 
                     D.UID, 
                     D.typeid, 
                      (CASE 
                        WHEN datediff('minute', 
                           (CASE 
                              WHEN (D.EndDate = D.Applytime) THEN D.Applytime
                              ELSE now()
                           END)::timestamp, D.EndDate) <> 0 THEN datediff('hour', now()::timestamp, D.EndDate)
                        ELSE -1
                     END)::int AS Ano, 
                      /*ISNULL(E.c0,*/ D.C0/*) AS C0*/, 
                      /*ISNULL(E.c1,*/ D.C1/*) AS C1*/, 
                      /*ISNULL(E.c2,*/ D.C2/*) AS C2*/, 
                      /*ISNULL(E.c3,*/ D.C3/*) AS C3*/, 
                      /*ISNULL(E.c4,*/ D.C4/*) AS C4*/, 
                      (CASE 
                        WHEN datediff('minute', D.Applytime, D.EndDate) > 0 THEN public."UNIX_TIMESTAMP"(CURRENT_TIMESTAMP)
                        ELSE 0
                     END) AS APPLY_DATE, 
                      (CASE 
                        WHEN datediff('minute', D.Applytime, D.EndDate) > 0 THEN public."UNIX_TIMESTAMP"(CURRENT_TIMESTAMP)
                        ELSE 0
                     END) AS END_DATE, 
                     D.Gift_flag, 
                      CASE 
                        WHEN (D.EndDate <= now() AND D.Applytime <> D.EndDate) THEN D.flag + 2
                        ELSE D.flag
                     END AS flag, 
                     D.Purchase, 
                     D.ItemType, 
                     D.ClubSet_WorkShop_Flag, 
                     D.ClubSet_WorkShop_C0, 
                     D.ClubSet_WorkShop_C1, 
                     D.ClubSet_WorkShop_C2, 
                     D.ClubSet_WorkShop_C3, 
                     D.ClubSet_WorkShop_C4, 
                     D.Mastery_Pts, 
                     D.Recovery_Pts, 
                     D.Level, 
                     D.Up
                  FROM 
                     (
                        SELECT 
                           pangya_item_warehouse.item_id, 
                           pangya_item_warehouse.UID, 
                           pangya_item_warehouse.typeid, 
                           pangya_item_warehouse.valid, 
                           pangya_item_warehouse.regdate, 
                           pangya_item_warehouse.Gift_flag, 
                           pangya_item_warehouse.flag, 
                           pangya_item_warehouse.Applytime, 
                           pangya_item_warehouse.EndDate, 
                           pangya_item_warehouse.C0, 
                           pangya_item_warehouse.C1, 
                           pangya_item_warehouse.C2, 
                           pangya_item_warehouse.C3, 
                           pangya_item_warehouse.C4, 
                           pangya_item_warehouse.Purchase, 
                           pangya_item_warehouse.ItemType, 
                           pangya_item_warehouse.ClubSet_WorkShop_Flag, 
                           pangya_item_warehouse.ClubSet_WorkShop_C0, 
                           pangya_item_warehouse.ClubSet_WorkShop_C1, 
                           pangya_item_warehouse.ClubSet_WorkShop_C2, 
                           pangya_item_warehouse.ClubSet_WorkShop_C3, 
                           pangya_item_warehouse.ClubSet_WorkShop_C4, 
                           pangya_item_warehouse.Mastery_Pts, 
                           pangya_item_warehouse.Recovery_Pts, 
                           pangya_item_warehouse.Level, 
                           pangya_item_warehouse.Up, 
                           pangya_item_warehouse.Total_Mastery_Pts, 
                           pangya_item_warehouse.Mastery_Gasto
                        FROM pangya.pangya_item_warehouse
                        WHERE pangya_item_warehouse.UID = _IDUSER AND pangya_item_warehouse.Gift_flag = 0
                     )  AS D 
                        --LEFT OUTER JOIN 
                        --(
                        --   SELECT 
                        --      pangya_clubset_enchant.uid, 
                        --      pangya_clubset_enchant.item_id, 
                        --      pangya_clubset_enchant.pang, 
                        --      pangya_clubset_enchant.c0, 
                        --      pangya_clubset_enchant.c1, 
                        --      pangya_clubset_enchant.c2, 
                        --      pangya_clubset_enchant.c3, 
                        --      pangya_clubset_enchant.c4
                        --   FROM pangya.pangya_clubset_enchant
                        --   WHERE pangya_clubset_enchant.uid = @IDUSER
                        --)  AS E 
                        --ON D.item_id = E.item_id                  

               )  AS R 
                  INNER JOIN 
                  (
                     SELECT 
                        F.item_id, 
                        G.UCC_NAME, 
                        G.UCCIDX, 
                        G.SEQ, 
                        G.COPIER, 
                        G.COPIER_NICK, 
                        G.TRADE, 
                        G.Flag, 
                        G.STATUS
                     FROM 
                        (
                           SELECT pangya_item_warehouse.item_id
                           FROM pangya.pangya_item_warehouse
                           WHERE pangya_item_warehouse.UID = _IDUSER AND pangya_item_warehouse.Gift_flag = 0
                        )  AS F 
                           LEFT OUTER JOIN 
                           (
                              SELECT 
                                 tu_ucc.UID, 
                                 tu_ucc.TYPEID, 
                                 tu_ucc.UCCIDX, 
                                 tu_ucc.SEQ, 
                                 tu_ucc.UCC_NAME, 
                                 tu_ucc.USE_YN, 
                                 tu_ucc.IN_DATE, 
                                 tu_ucc.COPIER, 
                                 tu_ucc.ITEM_ID, 
                                 tu_ucc.COPIER_NICK, 
                                 tu_ucc.DRAW_DT, 
                                 tu_ucc.STATUS, 
                                 tu_ucc.Flag, 
                                 tu_ucc.SKEY, 
                                 tu_ucc.TRADE
                              FROM pangya.tu_ucc
                           )  AS G 
                           ON F.item_id = G.ITEM_ID
                     --GROUP BY F.item_id
                  )  AS L 
                  ON R.item_id = L.ITEM_ID
            --GROUP BY R.item_id
         )  AS A 
            INNER JOIN 
            (

               SELECT 
                  X.item_id, 
                   0 AS CARD_CHARACTER_1, 
                   0 AS CARD_CHARACTER_2, 
                   0 AS CARD_CHARACTER_3, 
                   0 AS CARD_CHARACTER_4, 
                   0 AS CARD_CADDIE_1, 
                   0 AS CARD_CADDIE_2, 
                   0 AS CARD_CADDIE_3, 
                   0 AS CARD_CADDIE_4, 
                   0 AS CARD_NPC_1, 
                   0 AS CARD_NPC_2, 
                   0 AS CARD_NPC_3, 
                   0 AS CARD_NPC_4
               FROM 
                  (
                     SELECT pangya_item_warehouse.item_id
                     FROM pangya.pangya_item_warehouse
                     WHERE 
                        pangya_item_warehouse.UID = _IDUSER AND 
                        pangya_item_warehouse.Gift_flag = 0 AND 
                        pangya_item_warehouse.item_id = _IDITEM
                  )  AS X 
                     LEFT OUTER JOIN 
                     (
                        SELECT pangya_card_equip.Tipo, pangya_card_equip.Slot, pangya_card_equip.card_typeid, pangya_card_equip.parts_id
                        FROM pangya.pangya_card_equip
                        WHERE pangya_card_equip.UID = _IDUSER AND pangya_card_equip.USE_YN = 1
                     )  AS Y 
                     ON Y.parts_id = X.item_id
              -- GROUP BY X.item_id               

            )  AS B 
            ON A.item_id = B.item_id
         ORDER BY A.typeid;
END;
$$;


ALTER FUNCTION pangya.procgetwarehouseitem_one(_iduser bigint, _iditem bigint) OWNER TO postgres;

--
-- TOC entry 703 (class 1255 OID 23387)
-- Name: procgrandprixclearplayer(integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procgrandprixclearplayer(_iduser integer, _typeid_in integer, _flag_in integer) RETURNS TABLE("_PLACE_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_PLACE int = 0;
BEGIN

	IF 
	(
		SELECT pangya_grandprix_clear.typeid
		FROM pangya.pangya_grandprix_clear
		WHERE pangya_grandprix_clear.uid = _IDUSER AND pangya_grandprix_clear.typeid = _typeid_in
	) IS NULL THEN
	BEGIN

		INSERT INTO pangya.pangya_grandprix_clear(pangya.pangya_grandprix_clear.uid, pangya.pangya_grandprix_clear.typeid, pangya.pangya_grandprix_clear.flag)
		VALUES (_IDUSER, _TYPEID_IN, _FLAG_IN);

		_PLACE = _FLAG_IN;

	END;
	ELSE
		IF 
		(
			SELECT pangya_grandprix_clear.flag
			FROM pangya.pangya_grandprix_clear
			WHERE pangya_grandprix_clear.uid = _IDUSER AND pangya_grandprix_clear.typeid = _typeid_in
		) > _flag_in THEN

			UPDATE pangya.pangya_grandprix_clear
				SET 
					FLAG = _FLAG_IN
			WHERE pangya_grandprix_clear.uid = _IDUSER AND pangya_grandprix_clear.typeid = _TYPEID_IN;

			_PLACE = _FLAG_IN;

		END IF;
	END IF;

	RETURN QUERY SELECT _PLACE;
END;
$$;


ALTER FUNCTION pangya.procgrandprixclearplayer(_iduser integer, _typeid_in integer, _flag_in integer) OWNER TO postgres;

--
-- TOC entry 704 (class 1255 OID 23388)
-- Name: procguildacceptmark(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procguildacceptmark(_guild_uid integer) RETURNS TABLE("_RET_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_RET INT = -1;
BEGIN

	IF _GUILD_UID > 0 THEN
	DECLARE
		_GUILD_MASTER INT = (SELECT GUILD_LEADER FROM pangya.pangya_guild WHERE GUILD_UID = _GUILD_UID);
		_GUILD_NEW_MARK INT = (SELECT GUILD_NEW_MARK_IDX FROM pangya.pangya_guild WHERE GUILD_UID = _GUILD_UID);
	BEGIN

		IF _GUILD_NEW_MARK IS NOT NULL AND _GUILD_NEW_MARK > 0 THEN
		BEGIN

			IF _GUILD_MASTER IS NOT NULL THEN

				UPDATE
					pangya.pangya_guild
						SET GUILD_STATE = (CASE WHEN GUILD_MARK_IMG_IDX = 0 AND GUILD_STATE = 0/*pending*/ THEN 1/*Now Open*/ ELSE GUILD_STATE END),
							GUILD_ACCEPT_DATE = (CASE WHEN GUILD_MARK_IMG_IDX = 0 AND GUILD_STATE = 0/*pending*/ THEN now()/*Accept Guild*/ ELSE GUILD_ACCEPT_DATE END),
							GUILD_MARK_IMG = CONCAT(GUILD_UID, '_', _GUILD_NEW_MARK),	-- Guild Mark Img, a string montada já
							GUILD_MARK_IMG_IDX = _GUILD_NEW_MARK,
							GUILD_NEW_MARK_IDX = 0
				WHERE GUILD_UID = _GUILD_UID;

				-- Insert Activity Player
				INSERT INTO pangya.pangya_guild_atividade_player(GUILD_UID, UID, FLAG)
				VALUES(_GUILD_UID, _GUILD_MASTER, 15);

				-- Sucesso
				_RET = 1;

			END IF;

		END;
		ELSE
			_RET = -2;	-- Não tem um new mark para ser aceito
		END IF;
	END;
	END IF;
	
	RETURN QUERY SELECT _RET AS RET;
END;
$$;


ALTER FUNCTION pangya.procguildacceptmark(_guild_uid integer) OWNER TO postgres;

--
-- TOC entry 705 (class 1255 OID 23389)
-- Name: procguildaceitapedido(integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procguildaceitapedido(_owner_uid integer, _iduser integer, _uid_guild integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN

	UPDATE pangya.pangya_guild_member
		SET 
		MEMBER_STATE_FLAG = 3
	WHERE GUILD_UID = _UID_GUILD AND MEMBER_UID = _IDUSER;

	INSERT INTO pangya.pangya_guild_atividade_player(UID, GUILD_UID, FLAG)
		VALUES (_IDUSER, _UID_GUILD, 3);

	-- Cria log de update para o Game Server verificar e pedir para o Message Server Atualize os Membros do Club
	INSERT INTO pangya.pangya_guild_update_activity(GUILD_UID, OWNER_UPDATE, PLAYER_UID, TYPE_UPDATE) VALUES(_UID_GUILD, _OWNER_UID, _IDUSER, 0/*Accept Member*/);
END;
$$;


ALTER FUNCTION pangya.procguildaceitapedido(_owner_uid integer, _iduser integer, _uid_guild integer) OWNER TO postgres;

--
-- TOC entry 706 (class 1255 OID 23390)
-- Name: procguildblock(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procguildblock(_guild_uid integer) RETURNS TABLE("_RET_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_RET INT = -1;
BEGIN

	IF _GUILD_UID > 0 THEN

		IF (SELECT GUILD_UID FROM pangya.pangya_guild WHERE GUILD_UID = _GUILD_UID AND GUILD_STATE IN(0, 1, 2)) IS NOT NULL THEN
		DECLARE
			_GUILD_MASTER INT = (SELECT GUILD_LEADER FROM pangya.pangya_guild WHERE GUILD_UID = _GUILD_UID);
		BEGIN

			IF _GUILD_MASTER IS NOT NULL THEN

				UPDATE
					pangya.pangya_guild
						SET GUILD_STATE = 3 -- GM bloqueou a Guild
				WHERE GUILD_UID = _GUILD_UID;

				-- Insert Activity
				INSERT INTO pangya.pangya_guild_atividade_player(GUILD_UID, UID, FLAG)
				VALUES(_GUILD_UID, _GUILD_MASTER, 16);

				-- Sucesso
				_RET = 1;

			END IF;

		END;
		ELSE
			_RET = -2;	-- A guild já está bloqueada
		END IF;

	END IF;

	RETURN QUERY SELECT _RET AS RET;
END;
$$;


ALTER FUNCTION pangya.procguildblock(_guild_uid integer) OWNER TO postgres;

--
-- TOC entry 707 (class 1255 OID 23391)
-- Name: procguildcancelapedido(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procguildcancelapedido(_iduser integer, _uid_guild integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN

	UPDATE pangya.account
         SET 
            GUILD_UID = 0
      WHERE UID = _IDUSER;

      DELETE 
      FROM pangya.pangya_guild_member
      WHERE GUILD_UID = _UID_GUILD AND MEMBER_UID = _IDUSER;

      INSERT INTO pangya.pangya_guild_atividade_player(UID, GUILD_UID, FLAG)
         VALUES (_IDUSER, _UID_GUILD, 2);
END;
$$;


ALTER FUNCTION pangya.procguildcancelapedido(_iduser integer, _uid_guild integer) OWNER TO postgres;

--
-- TOC entry 708 (class 1255 OID 23392)
-- Name: procguildchangeauthoritymember(integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procguildchangeauthoritymember(_guild_uid integer, _member_uid integer, _member_authority integer) RETURNS TABLE("_RET_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	-- Ret value
	_RET INT = -1;
BEGIN

	IF _GUILD_UID > 0 AND _MEMBER_UID > 0 AND _MEMBER_AUTHORITY >= 0 THEN
	DECLARE
		_FLAG INT = (SELECT MEMBER_STATE_FLAG FROM pangya.pangya_guild_member WHERE GUILD_UID = _GUILD_UID AND MEMBER_UID = _MEMBER_UID);
	BEGIN
		
		IF _FLAG IS NOT NULL THEN

			IF _FLAG != _MEMBER_AUTHORITY THEN
			DECLARE
				_OLD_SUB_MASTER INT = (SELECT GUILD_SUB_MASTER FROM pangya.pangya_guild WHERE GUILD_UID = _GUILD_UID);
			BEGIN
			
				IF _OLD_SUB_MASTER IS NOT NULL THEN
					
					IF _MEMBER_AUTHORITY = 2 THEN -- SUB MASTER

						-- Update Member Authority
						UPDATE
							pangya.pangya_guild_member
								SET MEMBER_STATE_FLAG = _MEMBER_AUTHORITY
						WHERE 
							GUILD_UID = _GUILD_UID AND MEMBER_UID = _MEMBER_UID;

						-- Update Guild Sub Master
						UPDATE
							pangya.pangya_guild
								SET GUILD_SUB_MASTER = _MEMBER_UID
						WHERE GUILD_UID = _GUILD_UID;

						-- Activity member new Sub Master
						INSERT INTO pangya.pangya_guild_atividade_player(GUILD_UID, UID, FLAG)
						VALUES(_GUILD_UID, _MEMBER_UID, 12/*Promovido a subMaster*/);

						-- Verifica se tinha um Sub Master para atualizar o state dele
						IF _OLD_SUB_MASTER > 0 THEN
							
							UPDATE
								pangya.pangya_guild_member
									SET MEMBER_STATE_FLAG = 3 -- Member
							WHERE GUILD_UID = _GUILD_UID AND MEMBER_UID = _OLD_SUB_MASTER;

							-- Activity old Sub Master to member
							INSERT INTO pangya.pangya_guild_atividade_player(GUILD_UID, UID, FLAG)
							VALUES(_GUILD_UID, _OLD_SUB_MASTER, 13/*Despromovido*/);
						END IF;
					
					ELSE	-- Membro
						
						-- Update Member Authority
						UPDATE
							pangya.pangya_guild_member
								SET MEMBER_STATE_FLAG = _MEMBER_AUTHORITY
						WHERE 
							GUILD_UID = _GUILD_UID AND MEMBER_UID = _MEMBER_UID;

						-- Update Guild Sub Master
						UPDATE
							pangya.pangya_guild
								SET GUILD_SUB_MASTER = 0	-- Nenhum Master
						WHERE GUILD_UID = _GUILD_UID;

						-- Activity member new Sub Master
						INSERT INTO pangya.pangya_guild_atividade_player(GUILD_UID, UID, FLAG)
						VALUES(_GUILD_UID, _MEMBER_UID, 13/*Despromovido*/);

					END IF;

					_RET = 1;	-- Trocou autoridade do membro com sucesso

				END IF;

			END;
			ELSE
				_RET = -2;	-- Ele já é subMaster ou Membro
			END IF;
		END IF;
	END;
	END IF;

	RETURN QUERY SELECT _RET AS RET;
END;
$$;


ALTER FUNCTION pangya.procguildchangeauthoritymember(_guild_uid integer, _member_uid integer, _member_authority integer) OWNER TO postgres;

--
-- TOC entry 711 (class 1255 OID 23393)
-- Name: procguildclosure(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procguildclosure(_guild_uid integer, _master_uid integer) RETURNS TABLE("_RET_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	-- Ret value
	_RET INT = -1;
BEGIN

	IF _GUILD_UID > 0 AND _MASTER_UID >= 0 THEN

		IF (SELECT GUILD_UID FROM pangya.pangya_guild WHERE GUILD_UID = _GUILD_UID AND ((_MASTER_UID = 0 AND GUILD_STATE IN(0, 1, 2, 3)) OR (_MASTER_UID > 0 AND GUILD_STATE IN(0, 1, 2)))) IS NOT NULL THEN
		DECLARE
			-- Dias que a guild vai ficar em estado de fechamento antes de fechar
			_DIAS INT = 7;
		BEGIN

			IF _MASTER_UID = 0 THEN	-- GM Fechou a guild
			DECLARE
				_GUILD_MASTER INT = (SELECT GUILD_LEADER FROM pangya.pangya_guild WHERE GUILD_UID = _GUILD_UID);
			BEGIN
			
				IF _GUILD_MASTER IS NOT NULL THEN

					UPDATE
						pangya.pangya_guild
							SET GUILD_STATE = 5, -- GM FECHOU A GUILD
								GUILD_CLOSURE_DATE = (now() + ('1 day'::interval * _DIAS))
					WHERE GUILD_UID = _GUILD_UID;

					-- Insert Activity Player
					INSERT INTO pangya.pangya_guild_atividade_player(GUILD_UID, UID, FLAG)
					VALUES(_GUILD_UID, _GUILD_MASTER, 17);

					-- Sucesso
					_RET = 1;

				END IF;

			END;
			ELSE
			
				UPDATE
					pangya.pangya_guild
						SET GUILD_STATE = 4,	-- Player fechou a guild
							GUILD_CLOSURE_DATE = (now() + ('1 day'::interval * _DIAS))
				WHERE GUILD_UID = _GUILD_UID AND GUILD_LEADER = _MASTER_UID;

				-- Insert Activity Player
				INSERT INTO pangya.pangya_guild_atividade_player(GUILD_UID, UID, FLAG)
				VALUES(_GUILD_UID, _MASTER_UID, 9);

				-- Sucesso
				_RET = 1;
					
			END IF;
		END;
		ELSE
			_RET = -2;	-- A guild já está fechada
		END IF;

	END IF;

	RETURN QUERY SELECT _RET AS RET;
END;
$$;


ALTER FUNCTION pangya.procguildclosure(_guild_uid integer, _master_uid integer) OWNER TO postgres;

--
-- TOC entry 712 (class 1255 OID 23394)
-- Name: procguildkickmember(integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procguildkickmember(_owner_uid integer, _member_uid integer, _guild_uid integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN

	IF _GUILD_UID > 0 AND _MEMBER_UID > 0 THEN
	DECLARE
		_FLAG INT = (SELECT MEMBER_STATE_FLAG FROM pangya.pangya_guild_member WHERE GUILD_UID = _GUILD_UID AND MEMBER_UID = _MEMBER_UID);
	BEGIN

		IF _FLAG IS NOT NULL THEN

			IF _FLAG = 2 THEN -- SUB MASTER
				UPDATE pangya.pangya_guild SET GUILD_SUB_MASTER = 0 WHERE GUILD_UID = _GUILD_UID;
			END IF;

			IF _FLAG = 9 THEN
				_FLAG = 4; -- Rejeita pedido para entrar na guild
			ELSE
				_FLAG = 6; -- Kick Member (Expulsa)
			END IF;
		
			-- Delete from table members of guild
			DELETE FROM
				pangya.pangya_guild_member
			WHERE 
				GUILD_UID = _GUILD_UID AND MEMBER_UID = _MEMBER_UID;

			-- Update Guild of player in table account
			UPDATE
				pangya.account
					SET Guild_UID = 0
			WHERE UID = _MEMBER_UID;

			-- Insert Ativity of player in table guild atividade
			INSERT INTO pangya.pangya_guild_atividade_player(GUILD_UID, UID, FLAG)
			VALUES(_GUILD_UID, _MEMBER_UID, _FLAG);

			IF _FLAG = 6 THEN -- Kick Member (Expulsa)
				-- Cria log de update para o Game Server verificar e pedir para o Message Server Atualize os Membros do Club
				INSERT INTO pangya.pangya_guild_update_activity(GUILD_UID, OWNER_UPDATE, PLAYER_UID, TYPE_UPDATE) VALUES(_GUILD_UID, _OWNER_UID, _MEMBER_UID, 2/*Kick Member*/);
			END IF;
		END IF;

	END;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procguildkickmember(_owner_uid integer, _member_uid integer, _guild_uid integer) OWNER TO postgres;

--
-- TOC entry 713 (class 1255 OID 23395)
-- Name: procguildmovemaster(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procguildmovemaster(_guild_uid integer, _member_uid integer) RETURNS TABLE("_RET_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_RET INT = -1;
BEGIN

	IF _GUILD_UID > 0 AND _MEMBER_UID > 0 THEN
	DECLARE
		-- Verifica se o membro que vai virar master da guild tem o level de Rookie E pra cima
		_LEVEL INT = (SELECT level FROM pangya.user_info WHERE UID = _MEMBER_UID);
	BEGIN

		IF _LEVEL IS NOT NULL AND _LEVEL > 0 THEN
		DECLARE
			_OLD_MASTER INT = (SELECT GUILD_LEADER FROM pangya.pangya_guild WHERE GUILD_UID = _GUILD_UID);
			_OLD_SUB_MASTER INT = (SELECT GUILD_SUB_MASTER FROM pangya.pangya_guild WHERE GUILD_UID = _GUILD_UID);
		BEGIN

			IF _OLD_MASTER IS NOT NULL AND _OLD_SUB_MASTER IS NOT NULL THEN

				-- Update Master of Guild
				UPDATE
					pangya.pangya_guild
						SET GUILD_LEADER = _MEMBER_UID
					WHERE
						GUILD_UID = _GUILD_UID;

				-- Update Member state flag to Master
				UPDATE
					pangya.pangya_guild_member
						SET MEMBER_STATE_FLAG = 1 -- Master
				WHERE
					GUILD_UID = _GUILD_UID AND MEMBER_UID = _MEMBER_UID;

				-- Update Old Master to member
				UPDATE
					pangya.pangya_guild_member
						SET MEMBER_STATE_FLAG = 3 -- Member
				WHERE
					GUILD_UID = _GUILD_UID AND MEMBER_UID = _OLD_MASTER;

				-- Verifica se o membro que virou Master era Sub Master e tira o Sub Master da Guild
				IF _OLD_SUB_MASTER = _MEMBER_UID THEN

					UPDATE
						pangya.pangya_guild
							SET GUILD_SUB_MASTER = 0	-- Zera, não tem mais sub master, só master já que o master virou membro comun
					WHERE GUILD_UID = _GUILD_UID;

				END IF;

				-- Insert Activity of New Master
				INSERT INTO pangya.pangya_guild_atividade_player(GUILD_UID, UID, FLAG)
				VALUES(_GUILD_UID, _MEMBER_UID, 11/*Promovido a master*/);

				-- Insert Activity of Old Master to member
				INSERT INTO pangya.pangya_guild_atividade_player(GUILD_UID, UID, FLAG)
				VALUES(_GUILD_UID, _OLD_MASTER, 5/*Promote*/);

				-- Passou o master
				INSERT INTO pangya.pangya_guild_atividade_player(GUILD_UID, UID, FLAG)
				VALUES(_GUILD_UID, _OLD_MASTER, 10/*Passou o master*/);

				-- Sucesso
				_RET = 1;

			END IF;

		END;
		ELSE
			_RET = -2;
		END IF;

	END;
	END IF;

	RETURN QUERY SELECT _RET AS RET;
END;
$$;


ALTER FUNCTION pangya.procguildmovemaster(_guild_uid integer, _member_uid integer) OWNER TO postgres;

--
-- TOC entry 714 (class 1255 OID 23396)
-- Name: procguildrejectmark(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procguildrejectmark(_guild_uid integer) RETURNS TABLE("_RET_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_RET INT = -1;
BEGIN

	IF _GUILD_UID > 0 THEN
	DECLARE
		-- GM Rejeito o Guild Mark
		_GUILD_MASTER INT = (SELECT GUILD_LEADER FROM pangya.pangya_guild WHERE GUILD_UID = _GUILD_UID);
	BEGIN

		IF _GUILD_MASTER IS NOT NULL THEN

			UPDATE
				pangya.pangya_guild
					SET GUILD_NEW_MARK_IDX = 0
			WHERE GUILD_UID = _GUILD_UID;

			-- Insert Activity Player
			INSERT INTO pangya.pangya_guild_atividade_player(GUILD_UID, UID, FLAG)
			VALUES(_GUILD_UID, _GUILD_MASTER, 14);

			-- Sucesso
			_RET = 1;

		END IF;
	END;
	END IF;

	RETURN QUERY SELECT _RET AS RET;
END;
$$;


ALTER FUNCTION pangya.procguildrejectmark(_guild_uid integer) OWNER TO postgres;

--
-- TOC entry 715 (class 1255 OID 23397)
-- Name: procguildreopen(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procguildreopen(_guild_uid integer) RETURNS TABLE("_RET_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_RET INT = -1;
BEGIN

	IF _GUILD_UID > 0 THEN

		IF (SELECT GUILD_UID FROM pangya.pangya_guild WHERE GUILD_UID = _GUILD_UID AND GUILD_STATE = 3) IS NOT NULL THEN
		DECLARE
			_GUILD_MASTER INT = (SELECT GUILD_LEADER FROM pangya.pangya_guild WHERE GUILD_UID = _GUILD_UID);
		BEGIN

			IF _GUILD_MASTER IS NOT NULL THEN

				UPDATE
					pangya.pangya_guild
						SET GUILD_STATE = (CASE WHEN GUILD_MARK_IMG_IDX = 0 THEN 0 /*Pending*/ ELSE 1/*Now Open*/ END)
				WHERE GUILD_UID = _GUILD_UID;

				-- Insert Activity Player
				INSERT INTO pangya.pangya_guild_atividade_player(GUILD_UID, UID, FLAG)
				VALUES(_GUILD_UID, _GUILD_MASTER, 18);

				-- Sucesso
				_RET = 1;

			END IF;

		END;
		ELSE
			_RET = -2;	-- A guild não está bloqueada pra desbloquear
		END IF;

	END IF;

	RETURN QUERY SELECT _RET AS RET;
END;
$$;


ALTER FUNCTION pangya.procguildreopen(_guild_uid integer) OWNER TO postgres;

--
-- TOC entry 717 (class 1255 OID 23398)
-- Name: procguildreopen(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procguildreopen(_guild_uid integer, _master_uid integer) RETURNS TABLE("_RET_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_RET INT = -1;
BEGIN

	IF _GUILD_UID > 0 AND _MASTER_UID >= 0 THEN

		IF (SELECT GUILD_UID FROM pangya.pangya_guild WHERE GUILD_UID = _GUILD_UID 
			AND ((_MASTER_UID = 0 AND GUILD_STATE IN(4,5)) OR (_MASTER_UID > 0 AND GUILD_STATE = 4)) 
			AND (GUILD_CLOSURE_DATE IS NULL OR now() < GUILD_CLOSURE_DATE)) IS NOT NULL THEN
		BEGIN

			IF _MASTER_UID = 0 THEN -- GM reabriu a guild do player
			DECLARE
				_GUILD_MASTER INT = (SELECT GUILD_LEADER FROM pangya.pangya_guild WHERE GUILD_UID = _GUILD_UID);
			BEGIN

				IF _GUILD_MASTER IS NOT NULL THEN

					UPDATE
						pangya.pangya_guild
							SET GUILD_STATE = (CASE WHEN GUILD_MARK_IMG_IDX = 0 THEN 0 /*Pending*/ ELSE 1/*Now Open*/ END),
								GUILD_CLOSURE_DATE = NULL
					WHERE GUILD_UID = _GUILD_UID;

					-- Insert Activity Player
					INSERT INTO pangya.pangya_guild_atividade_player(GUILD_UID, UID, FLAG)
					VALUES(_GUILD_UID, _GUILD_MASTER, 20);

					-- Sucesso
					_RET = 1;

				END IF;

			END;
			ELSE
			
				UPDATE
					pangya.pangya_guild
						SET GUILD_STATE = (CASE WHEN GUILD_MARK_IMG_IDX = 0 THEN 0 /*Pending*/ ELSE 1/*Now Open*/ END),
							GUILD_CLOSURE_DATE = NULL
				WHERE GUILD_UID = _GUILD_UID AND GUILD_LEADER = _MASTER_UID;

				-- Insert Activity Player
				INSERT INTO pangya.pangya_guild_atividade_player(GUILD_UID, UID, FLAG)
				VALUES(_GUILD_UID, _MASTER_UID, 19);

				-- Sucesso
				_RET = 1;

			END IF;

		END;
		ELSE
			_RET = -2;	-- a guild não está fechada
		END IF;
	END IF;

	RETURN QUERY SELECT _RET AS RET;
END;
$$;


ALTER FUNCTION pangya.procguildreopen(_guild_uid integer, _master_uid integer) OWNER TO postgres;

--
-- TOC entry 718 (class 1255 OID 23399)
-- Name: procguildunblock(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procguildunblock(_guild_uid integer) RETURNS TABLE("_RET_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_RET INT = -1;
BEGIN
	IF _GUILD_UID > 0 THEN

		IF (SELECT GUILD_UID FROM pangya.pangya_guild WHERE GUILD_UID = _GUILD_UID AND GUILD_STATE = 3) IS NOT NULL THEN
		DECLARE
			_GUILD_MASTER INT = (SELECT GUILD_LEADER FROM pangya.pangya_guild WHERE GUILD_UID = _GUILD_UID);
		BEGIN

			IF _GUILD_MASTER IS NOT NULL THEN

				UPDATE
					pangya.pangya_guild
						SET GUILD_STATE = (CASE WHEN GUILD_MARK_IMG_IDX = 0 THEN 0 /*Pending*/ ELSE 1/*Now Open*/ END)
				WHERE GUILD_UID = _GUILD_UID;

				-- Insert Activity Player
				INSERT INTO pangya.pangya_guild_atividade_player(GUILD_UID, UID, FLAG)
				VALUES(_GUILD_UID, _GUILD_MASTER, 18);

				-- Sucesso
				_RET = 1;

			END IF;

		END;
		ELSE
			_RET = -2;	-- A guild não está bloqueada pra desbloquear
		END IF;

	END IF;

	RETURN QUERY SELECT _RET AS RET;
END;
$$;


ALTER FUNCTION pangya.procguildunblock(_guild_uid integer) OWNER TO postgres;

--
-- TOC entry 719 (class 1255 OID 23400)
-- Name: procinsertachievementall(integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procinsertachievementall(_typeid_achieve integer, _typeid_quest integer, _objectivo integer, _opt integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
DECLARE
	_IDUSER int;
	_ID_ACHIEVE int;
	_ID_COUNTER int;
	UID_CURSOR CURSOR FOR 
		SELECT account.UID
		FROM pangya.account;
BEGIN

	OPEN UID_CURSOR;
	
	WHILE (1 = 1) LOOP

		FETCH NEXT FROM UID_CURSOR
		INTO _IDUSER;

		-- exit
		exit when not found;

		INSERT INTO pangya.achievement_tipo(pangya.achievement_tipo.UID, pangya.achievement_tipo.TypeID, pangya.achievement_tipo.TIPO, pangya.achievement_tipo.Option)
		VALUES (_IDUSER, _TYPEID_ACHIEVE, 1, _OPT);

		SELECT max(achievement_tipo.ID_ACHIEVEMENT) INTO _ID_ACHIEVE
		FROM pangya.achievement_tipo
		WHERE achievement_tipo.UID = _IDUSER;

		INSERT INTO pangya.achievement_quest(
			pangya.achievement_quest.UID, 
			pangya.achievement_quest.ID_ACHIEVEMENT, 
			pangya.achievement_quest.TypeID_ACHIEVE, 
			pangya.achievement_quest.Count_ID, 
			pangya.achievement_quest.Data_Sec, 
			pangya.achievement_quest.Objetivo_Quest)
		VALUES (
			_IDUSER, 
			_ID_ACHIEVE, 
			_TYPEID_QUEST, 
			0, 
			0, 
			_OBJECTIVO);

	END LOOP;

	CLOSE UID_CURSOR;
END;
$$;


ALTER FUNCTION pangya.procinsertachievementall(_typeid_achieve integer, _typeid_quest integer, _objectivo integer, _opt integer) OWNER TO postgres;

--
-- TOC entry 720 (class 1255 OID 23401)
-- Name: procinsertachievementoneuser(integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procinsertachievementoneuser(_iduser integer, _typeid_achieve integer, _typeid_quest integer, _objectivo integer, _opt integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
DECLARE
	_ID_ACHIEVE int = 0;
BEGIN

	INSERT INTO pangya.achievement_tipo(pangya.achievement_tipo.UID, pangya.achievement_tipo.TypeID, pangya.achievement_tipo.TIPO, pangya.achievement_tipo.Option)
         VALUES (_IDUSER, _TYPEID_ACHIEVE, 1, _OPT);

      SELECT max(achievement_tipo.ID_ACHIEVEMENT) INTO _ID_ACHIEVE
      FROM pangya.achievement_tipo
      WHERE achievement_tipo.UID = _IDUSER;

      INSERT INTO pangya.achievement_quest(
         pangya.achievement_quest.UID, 
         pangya.achievement_quest.ID_ACHIEVEMENT, 
         pangya.achievement_quest.TypeID_ACHIEVE, 
         pangya.achievement_quest.Count_ID, 
         pangya.achievement_quest.Data_Sec, 
         pangya.achievement_quest.Objetivo_Quest)
         VALUES (
            _IDUSER, 
            _ID_ACHIEVE, 
            _TYPEID_QUEST, 
            0, 
            0, 
            _OBJECTIVO);
END;
$$;


ALTER FUNCTION pangya.procinsertachievementoneuser(_iduser integer, _typeid_achieve integer, _typeid_quest integer, _objectivo integer, _opt integer) OWNER TO postgres;

--
-- TOC entry 721 (class 1255 OID 23402)
-- Name: procinsertachievementquest(integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procinsertachievementquest(_iduser integer, _achievement_typeid integer, _achievement_quest_typeid integer, _counter_item_typeid integer, _objective integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
DECLARE
	_IDACHIEVE int;
	_COUNT_ITEM_ID int;
BEGIN

	SELECT achievement_tipo.ID_ACHIEVEMENT INTO _IDACHIEVE
      FROM pangya.achievement_tipo
      WHERE 
         achievement_tipo.UID = _IDUSER AND 
         achievement_tipo.TypeID = _ACHIEVEMENT_TYPEID AND 
         achievement_tipo.TIPO <> 2;

      IF 
         (
            SELECT max(count_item.Count_ID)
            FROM pangya.count_item
            WHERE count_item.UID = _IDUSER AND count_item.TypeID = _COUNTER_ITEM_TYPEID
         ) IS NULL THEN
         INSERT INTO pangya.count_item(pangya.count_item.UID, pangya.count_item.TypeID, pangya.count_item.TIPO)
            VALUES (_IDUSER, _COUNTER_ITEM_TYPEID, 1);
	END IF;

      SELECT max(count_item.Count_ID) INTO _COUNT_ITEM_ID
      FROM pangya.count_item
      WHERE count_item.UID = _IDUSER AND count_item.TypeID = _COUNTER_ITEM_TYPEID;

      INSERT INTO pangya.achievement_quest(
         pangya.achievement_quest.UID, 
         pangya.achievement_quest.ID_ACHIEVEMENT, 
         pangya.achievement_quest.TypeID_ACHIEVE, 
         pangya.achievement_quest.Count_ID, 
         pangya.achievement_quest.Data_Sec, 
         pangya.achievement_quest.Objetivo_Quest)
         VALUES (
            _IDUSER, 
            _IDACHIEVE, 
            _ACHIEVEMENT_QUEST_TYPEID, 
            _COUNT_ITEM_ID, 
            0, 
            _Objective);
END;
$$;


ALTER FUNCTION pangya.procinsertachievementquest(_iduser integer, _achievement_typeid integer, _achievement_quest_typeid integer, _counter_item_typeid integer, _objective integer) OWNER TO postgres;

--
-- TOC entry 722 (class 1255 OID 23403)
-- Name: procinsertblockip(text, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procinsertblockip(_ip text, _mask text) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN

	IF _IP != '' AND _MASK != '' THEN

		IF (SELECT index FROM pangya.pangya_ip_table WHERE ip = _IP AND mask = _MASK) IS NULL THEN
			INSERT INTO pangya.pangya_ip_table(ip, mask) VALUES(_IP, _MASK);
		END IF;

	END IF;
END;
$$;


ALTER FUNCTION pangya.procinsertblockip(_ip text, _mask text) OWNER TO postgres;

--
-- TOC entry 723 (class 1255 OID 23404)
-- Name: procinsertblockmac(text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procinsertblockmac(_mac text) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN

	IF _MAC != '' THEN
		INSERT INTO pangya.pangya_mac_table(mac) VALUES(_MAC);
	END IF;
END;
$$;


ALTER FUNCTION pangya.procinsertblockmac(_mac text) OWNER TO postgres;

--
-- TOC entry 687 (class 1255 OID 23405)
-- Name: procinsertbotgmeventreward(integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procinsertbotgmeventreward(_typeid integer, _qntd integer, _qntd_time integer, _rate integer, _valid integer) RETURNS TABLE("_ID_" bigint)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_id bigint = -1;
BEGIN

	IF _TYPEID > 0 THEN

		INSERT INTO pangya.pangya_bot_gm_event_reward(typeid, qntd, qntd_time, rate, valid)
		VALUES(_TYPEID, _QNTD, _QNTD_TIME, _RATE, _VALID)
		RETURNING index INTO _id;
	END IF;
	
	RETURN QUERY SELECT _id AS ID;
END;
$$;


ALTER FUNCTION pangya.procinsertbotgmeventreward(_typeid integer, _qntd integer, _qntd_time integer, _rate integer, _valid integer) OWNER TO postgres;

--
-- TOC entry 709 (class 1255 OID 23406)
-- Name: procinsertbotgmeventtime(text, text, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procinsertbotgmeventtime(_inicio_time text, _fim_time text, _channel_id integer, _valid integer) RETURNS TABLE("_ID_" bigint)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_id bigint = -1;
BEGIN

	IF _INICIO_TIME IS NOT NULL AND _FIM_TIME IS NOT NULL THEN

		INSERT INTO pangya.pangya_bot_gm_event_time(inicio_time, fim_time, channel_id, valid)
		VALUES(_INICIO_TIME::time, _FIM_TIME::time, _CHANNEL_ID, _VALID)
		RETURNING index INTO _id;
	END IF;
	
	RETURN QUERY SELECT _id AS ID;
END;
$$;


ALTER FUNCTION pangya.procinsertbotgmeventtime(_inicio_time text, _fim_time text, _channel_id integer, _valid integer) OWNER TO postgres;

--
-- TOC entry 710 (class 1255 OID 23407)
-- Name: procinsertboxrarewinlog(integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procinsertboxrarewinlog(_iduser integer, _box_typeid integer, _item_typeid integer, _qntd integer, _raridade integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	
	IF _IDUSER > 0 AND _BOX_TYPEID > 0 AND _ITEM_TYPEID> 0 THEN
		INSERT INTO pangya.pangya_new_box_rare_win_log(UID, box_typeid, item_typeid, qntd, raridade, win_date)
			VALUES(_IDUSER, _BOX_TYPEID, _ITEM_TYPEID, _QNTD, _RARIDADE, now());
	END IF;
END;
$$;


ALTER FUNCTION pangya.procinsertboxrarewinlog(_iduser integer, _box_typeid integer, _item_typeid integer, _qntd integer, _raridade integer) OWNER TO postgres;

--
-- TOC entry 716 (class 1255 OID 23408)
-- Name: procinsertcard(integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procinsertcard(_iduser integer, _itemtypeid integer, _qntd integer, _cardtype integer) RETURNS TABLE("_ITEM_ID_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_ITEM_ID int = -1;
BEGIN

	IF _ITEMTYPEID > 0 AND (SELECT UID FROM pangya.account WHERE UID = _IDUSER) IS NOT NULL THEN
		_ITEM_ID = COALESCE((SELECT card_itemid FROM pangya.pangya_card WHERE UID = _IDUSER AND card_typeid = _ITEMTYPEID LIMIT 1), -1);

		IF _ITEM_ID > 0 AND _ITEM_ID IS NOT NULL THEN
			UPDATE pangya.pangya_card SET QNTD = QNTD + _QNTD, USE_YN = 'N' WHERE UID = _IDUSER AND card_typeid = _ITEMTYPEID;
		ELSE
			INSERT INTO pangya.pangya_card(UID, card_typeid, card_type, QNTD, GET_DT, USE_YN)
				VALUES(_IDUSER, _ITEMTYPEID, _CARDTYPE, _QNTD, now(), 'N')
				RETURNING card_itemid INTO _ITEM_ID;
		END IF;
	END IF;

	RETURN QUERY SELECT _ITEM_ID AS ITEM_ID;
END;
$$;


ALTER FUNCTION pangya.procinsertcard(_iduser integer, _itemtypeid integer, _qntd integer, _cardtype integer) OWNER TO postgres;

--
-- TOC entry 724 (class 1255 OID 23409)
-- Name: procinsertclubset(integer, integer, integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procinsertclubset(_iduser integer, _giftflag integer, _purchase integer, _itemtypeid integer, _flag integer, _tempo integer, _workshop_level integer) RETURNS TABLE("_ITEM_ID_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_ITEM_ID INT = -1;
BEGIN
	IF _ITEMTYPEID > 0 AND (SELECT UID FROM pangya.account WHERE UID = _IDUSER) IS NOT NULL THEN
		IF _TEMPO > 0 THEN
			INSERT INTO pangya.pangya_item_warehouse(UID, typeid, valid, regdate, EndDate, Gift_flag, flag, Purchase, ItemType, Level)
				VALUES(_IDUSER, _ITEMTYPEID, 1, now(), (now() + ('1 day'::interval * _TEMPO)), _GIFTFLAG, _FLAG, _PURCHASE, 2, _WORKSHOP_LEVEL)
				RETURNING item_id INTO _ITEM_ID;
		ELSE
			INSERT INTO pangya.pangya_item_warehouse(UID, typeid, valid, regdate, Gift_flag, flag, Purchase, ItemType, Level)
				VALUES(_IDUSER, _ITEMTYPEID, 1, now(), _GIFTFLAG, _FLAG, _PURCHASE, 2, _WORKSHOP_LEVEL)
				RETURNING item_id INTO _ITEM_ID;
		END IF;

		INSERT INTO pangya.pangya_clubset_enchant(UID, item_id) VALUES(_IDUSER, _ITEM_ID);
	END IF;

	RETURN QUERY SELECT _ITEM_ID AS ITEM_ID;
END;
$$;


ALTER FUNCTION pangya.procinsertclubset(_iduser integer, _giftflag integer, _purchase integer, _itemtypeid integer, _flag integer, _tempo integer, _workshop_level integer) OWNER TO postgres;

--
-- TOC entry 725 (class 1255 OID 23410)
-- Name: procinsertcoincubelocation(integer, integer, integer, integer, bigint, double precision, double precision, double precision); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procinsertcoincubelocation(__course integer, __hole integer, __tipo integer, __tipo_location integer, __rate bigint, __x double precision, __y double precision, __z double precision) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	INSERT INTO pangya.pangya_coin_cube_location(course, hole, tipo, tipo_location, rate, x, y, z)
	VALUES(__COURSE, __HOLE, __TIPO, __TIPO_LOCATION, __RATE, __X, __Y, __Z);
END;
$$;


ALTER FUNCTION pangya.procinsertcoincubelocation(__course integer, __hole integer, __tipo integer, __tipo_location integer, __rate bigint, __x double precision, __y double precision, __z double precision) OWNER TO postgres;

--
-- TOC entry 726 (class 1255 OID 23411)
-- Name: procinsertcommand(integer, integer, integer, integer, integer, integer, integer, integer, integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procinsertcommand(_command_id integer, _arg1 integer, _arg2 integer, _arg3 integer, _arg4 integer, _arg5 integer, _target integer, _flag integer, _valid integer, _reservedate text) RETURNS TABLE("_IDX_" bigint)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_IDX bigint = -1;
BEGIN
	INSERT INTO pangya.pangya_command(command_id, arg1, arg2, arg3, arg4, arg5, target, flag, valid, reserveDate)
		VALUES(_COMMAND_ID, _ARG1, _ARG2, _ARG3, _ARG4, _ARG5, _TARGET, _FLAG, _VALID, _RESERVEDATE::timestamp)
		RETURNING idx INTO _IDX;

	RETURN QUERY SELECT _IDX AS IDX;
END;
$$;


ALTER FUNCTION pangya.procinsertcommand(_command_id integer, _arg1 integer, _arg2 integer, _arg3 integer, _arg4 integer, _arg5 integer, _target integer, _flag integer, _valid integer, _reservedate text) OWNER TO postgres;

--
-- TOC entry 727 (class 1255 OID 23412)
-- Name: procinsertcplog(integer, integer, integer, bigint, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procinsertcplog(_iduser integer, _type integer, _mail_id integer, _cookie bigint, _item_qnty integer) RETURNS TABLE("_IDX_" bigint)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_LOG_ID bigint = -1;
BEGIN
	IF _IDUSER > 0 THEN
		INSERT INTO pangya.pangya_cookie_point_log(UID, type, mail_id, cookie, item_qnty)
			VALUES(_IDUSER, _TYPE, _MAIL_ID, _COOKIE, _ITEM_QNTY)
			RETURNING id INTO _LOG_ID;
	END IF;

	RETURN QUERY SELECT _LOG_ID as id;
END;
$$;


ALTER FUNCTION pangya.procinsertcplog(_iduser integer, _type integer, _mail_id integer, _cookie bigint, _item_qnty integer) OWNER TO postgres;

--
-- TOC entry 728 (class 1255 OID 23413)
-- Name: procinsertcplogitem(bigint, integer, integer, bigint); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procinsertcplogitem(_cp_log_id bigint, _typeid integer, _qnty integer, _price bigint) RETURNS TABLE("_IDX_" bigint)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_INDEX bigint = -1;
BEGIN
	IF _CP_LOG_ID > 0 THEN
		INSERT INTO pangya.pangya_cookie_point_item_log(cp_id_log, typeid, qnty, price)
			VALUES(_CP_LOG_ID, _TYPEID, _QNTY, _PRICE)
			RETURNING index INTO _INDEX;
	END IF;

	RETURN QUERY SELECT _INDEX AS index;
END;
$$;


ALTER FUNCTION pangya.procinsertcplogitem(_cp_log_id bigint, _typeid integer, _qnty integer, _price bigint) OWNER TO postgres;

--
-- TOC entry 729 (class 1255 OID 23414)
-- Name: procinsertdailyquest_new(integer, text, integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procinsertdailyquest_new(_iduser integer, _quest_nome text, _quest_typeid integer, __tipo integer, __option integer, _stuff_typeid integer, _count_objective integer) RETURNS TABLE("_ID_ACHIEVEMENT_" bigint)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_ID_ACHIEVE BIGINT = -1;
BEGIN
	INSERT INTO pangya.achievement_tipo(
         pangya.achievement_tipo.UID, 
         pangya.achievement_tipo.Nome, 
         pangya.achievement_tipo.TypeID, 
         pangya.achievement_tipo.TIPO, 
         pangya.achievement_tipo.Option)
         VALUES (
            _IDUSER, 
            _quest_nome, 
            _quest_typeid, 
            __tipo, 
            __option)
		RETURNING ID_ACHIEVEMENT INTO _ID_ACHIEVE;
		
	INSERT INTO pangya.achievement_quest(
         pangya.achievement_quest.UID, 
         pangya.achievement_quest.ID_ACHIEVEMENT, 
         pangya.achievement_quest.TypeID_ACHIEVE, 
         pangya.achievement_quest.Count_ID, 
         pangya.achievement_quest.Data_Sec, 
         pangya.achievement_quest.Objetivo_Quest)
         VALUES (
            _IDUSER, 
            _ID_ACHIEVE, 
            _stuff_typeid, 
            0, 
            0, 
            _count_objective);
			
	RETURN QUERY SELECT _ID_ACHIEVE AS ID_ACHIEVEMENT;
END;
$$;


ALTER FUNCTION pangya.procinsertdailyquest_new(_iduser integer, _quest_nome text, _quest_typeid integer, __tipo integer, __option integer, _stuff_typeid integer, _count_objective integer) OWNER TO postgres;

--
-- TOC entry 730 (class 1255 OID 23415)
-- Name: procinsertdonationnew(integer, integer, text, text, text, text, integer, integer, text, double precision, double precision, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procinsertdonationnew(_uid integer, _plataforma integer, _email text, _date text, _update text, _code text, _type integer, _status integer, _reference text, _gross_amount double precision, _net_amount double precision, _escrow text) RETURNS TABLE("_ID_" bigint)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_ID BIGINT = -1;
BEGIN
	IF _CODE != '' AND _EMAIL != '' AND _DATE IS NOT NULL AND _REFERENCE != '' THEN

		INSERT INTO pangya.pangya_donation_new(UID, PLATAFORMA, EMAIL, DATE, UPDATE, CODE, TYPE, STATUS, REFERENCE, GROSS_AMOUNT, NET_AMOUNT, ESCROW)
		VALUES(_UID, _PLATAFORMA, _EMAIL, _DATE::timestamp, _UPDATE::timestamp, _CODE, _TYPE, _STATUS, _REFERENCE, _GROSS_AMOUNT, _NET_AMOUNT, _ESCROW::timestamp)
		RETURNING index INTO _ID;
	END IF;
	
	RETURN QUERY SELECT _ID AS ID;
END;
$$;


ALTER FUNCTION pangya.procinsertdonationnew(_uid integer, _plataforma integer, _email text, _date text, _update text, _code text, _type integer, _status integer, _reference text, _gross_amount double precision, _net_amount double precision, _escrow text) OWNER TO postgres;

--
-- TOC entry 731 (class 1255 OID 23416)
-- Name: procinsertgoldentimeevent(integer, text, text, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procinsertgoldentimeevent(_type integer, _begin text, _end text, _rate integer, _is_end integer) RETURNS TABLE("_ID_" bigint)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_ID BIGINT = -1;
BEGIN
	IF _BEGIN IS NOT NULL THEN

		INSERT INTO pangya.pangya_golden_time_info(type, begin, "end", rate, is_end)
		VALUES(_TYPE, _BEGIN::date, _END::date, _RATE, _IS_END)
		RETURNING index INTO _ID;

	END IF;
	
	RETURN QUERY SELECT _ID AS ID;
END;
$$;


ALTER FUNCTION pangya.procinsertgoldentimeevent(_type integer, _begin text, _end text, _rate integer, _is_end integer) OWNER TO postgres;

--
-- TOC entry 732 (class 1255 OID 23417)
-- Name: procinsertgoldentimeitem(bigint, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procinsertgoldentimeitem(_golden_time_id bigint, _typeid integer, _qntd integer, _qntd_time integer, _rate integer) RETURNS TABLE("_ID_" bigint)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_ID BIGINT = -1;
BEGIN
	IF _GOLDEN_TIME_ID > 0 AND _TYPEID > 0 THEN

		INSERT INTO pangya.pangya_golden_time_item(golden_time_id, typeid, qntd, qntd_time, rate)
		VALUES(_GOLDEN_TIME_ID, _TYPEID, _QNTD, _QNTD_TIME, _RATE)
		RETURNING index INTO _ID;
	END IF;
	
	RETURN QUERY SELECT _ID AS ID;
END;
$$;


ALTER FUNCTION pangya.procinsertgoldentimeitem(_golden_time_id bigint, _typeid integer, _qntd integer, _qntd_time integer, _rate integer) OWNER TO postgres;

--
-- TOC entry 465 (class 1255 OID 23418)
-- Name: procinsertgoldentimeround(bigint, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procinsertgoldentimeround(_golden_time_id bigint, _time text) RETURNS TABLE("_ID_" bigint)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_ID BIGINT = -1;
BEGIN
	IF _GOLDEN_TIME_ID > 0 AND _TIME IS NOT NULL THEN

		INSERT INTO pangya.pangya_golden_time_round(golden_time_id, time)
		VALUES(_GOLDEN_TIME_ID, _TIME::time)
		RETURNING index INTO _ID;

	END IF;
	
	RETURN QUERY SELECT _ID AS ID;
END;
$$;


ALTER FUNCTION pangya.procinsertgoldentimeround(_golden_time_id bigint, _time text) OWNER TO postgres;

--
-- TOC entry 579 (class 1255 OID 23419)
-- Name: procinsertitemnoemail(integer, integer, integer, integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procinsertitemnoemail(_from_uid integer, _to_uid integer, _idmsg integer, _iditem integer, _typeiditem integer, _opt integer, _qntd integer, _qntd_dia integer) RETURNS TABLE("_MSG_ID_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE 
	_GRUPO INT;
	_UCC_IDX VARCHAR(8) default '0';
BEGIN
	INSERT INTO pangya.pangya_item_mail(MSG_ID, ITEM_ID, ITEM_TYPEID, FLAG, GET_DATE, QUANTIDADE_ITEM, QUANTIDADE_DIA, VALID, UCC_IMG_MARK)
    VALUES(_IDMSG, _IDITEM, _TYPEIDITEM, _OPT, now(), _QNTD, _QNTD_DIA, 1, _UCC_IDX);

    RETURN QUERY SELECT _IDMSG as MSG_ID;
END;
$$;


ALTER FUNCTION pangya.procinsertitemnoemail(_from_uid integer, _to_uid integer, _idmsg integer, _iditem integer, _typeiditem integer, _opt integer, _qntd integer, _qntd_dia integer) OWNER TO postgres;

--
-- TOC entry 733 (class 1255 OID 23420)
-- Name: procinsertloginrewardevent(text, integer, integer, integer, integer, integer, integer, integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procinsertloginrewardevent(_name text, _type integer, _days_to_gift integer, _n_times_gift integer, _item_typeid integer, _item_qntd integer, _item_qntd_time integer, _is_end integer, _end_date text) RETURNS TABLE("_ID_" bigint)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_ID BIGINT = -1;
BEGIN
	IF _NAME != '' THEN

		INSERT INTO pangya.pangya_login_reward(name, type, days_to_gift, n_times_gift, item_typeid, item_qntd, item_qntd_time, is_end, end_date)
		VALUES(_NAME, _TYPE, _DAYS_TO_GIFT, _N_TIMES_GIFT, _ITEM_TYPEID, _ITEM_QNTD, _ITEM_QNTD_TIME, _IS_END, _END_DATE::timestamp)
		RETURNING index INTO _ID;
	END IF;
	
	RETURN QUERY SELECT _ID AS ID;
END;
$$;


ALTER FUNCTION pangya.procinsertloginrewardevent(_name text, _type integer, _days_to_gift integer, _n_times_gift integer, _item_typeid integer, _item_qntd integer, _item_qntd_time integer, _is_end integer, _end_date text) OWNER TO postgres;

--
-- TOC entry 734 (class 1255 OID 23421)
-- Name: procinsertloginrewardplayer(bigint, integer, integer, integer, integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procinsertloginrewardplayer(_id bigint, _uid integer, _count_days integer, _count_seq integer, _is_clear integer, _update_date text) RETURNS TABLE("_IDX_" bigint)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_IDX BIGINT = 0;
BEGIN
	IF _ID > 0 AND _UID > 0 AND (SELECT index FROM pangya.pangya_login_reward WHERE index = _ID) IS NOT NULL THEN
		INSERT INTO pangya.pangya_login_reward_player(UID, LOGIN_REWARD_ID, COUNT_DAYS, COUNT_SEQ, IS_CLEAR, UPDATE_DATE)
		VALUES(_UID, _ID, _COUNT_DAYS, _COUNT_SEQ, _IS_CLEAR, _UPDATE_DATE::timestamp)
		RETURNING index INTO _IDX;
	END IF;
	
	RETURN QUERY SELECT _IDX AS IDX;
END;
$$;


ALTER FUNCTION pangya.procinsertloginrewardplayer(_id bigint, _uid integer, _count_days integer, _count_seq integer, _is_clear integer, _update_date text) OWNER TO postgres;

--
-- TOC entry 735 (class 1255 OID 23422)
-- Name: procinsertmascot(integer, integer, integer, integer, integer, text, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procinsertmascot(_iduser integer, _itemtypeid integer, _tipo integer, _iscash integer, _tempo integer, _message text, _price integer) RETURNS TABLE("_ITEM_ID_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_ITEM_ID INT = -1;
BEGIN
	IF _ITEMTYPEID > 0 AND (SELECT UID FROM pangya.account WHERE UID = _IDUSER) IS NOT NULL THEN
	DECLARE
		_regDT timestamp = now();
		_endDT timestamp = _regDT;
	BEGIN

		_ITEM_ID = (SELECT item_id FROM pangya.pangya_mascot_info WHERE UID = _IDUSER AND typeid = _ITEMTYPEID AND valid = 1);

		IF _ITEM_ID > 0 AND _ITEM_ID IS NOT NULL THEN
		DECLARE
			_diff INT = 0;
		BEGIN
			
			SELECT (case when DATEDIFF('second', now()::timestamp, EndDate) > 0 THEN 1 ELSE 0 END) INTO _diff FROM pangya.pangya_mascot_info WHERE UID = _IDUSER AND item_id = _ITEM_ID;
			 
			UPDATE pangya.pangya_mascot_info SET Message = _MESSAGE, EndDate = ((CASE WHEN _diff = 1 THEN EndDate ELSE now()::timestamp END) + ('1 day'::interval * _TEMPO))
			WHERE UID = _IDUSER AND item_id = _ITEM_ID;
		END;
		ELSE

			IF _TEMPO > 0 THEN
				_endDT = (_regDT + ('1 day'::interval * _TEMPO));
			END IF;

			INSERT INTO pangya.pangya_mascot_info(UID, typeid, valid, tipo, IsCash, RegDate, EndDate, Period, Message, Price)
				VALUES(_IDUSER, _ITEMTYPEID, 1, _TIPO, _ISCASH, _regDT, _endDT, _TEMPO, _MESSAGE, _PRICE)
				RETURNING item_id INTO _ITEM_ID;
		END IF;
	END;
	END IF;

	RETURN QUERY SELECT _ITEM_ID AS ITEM_ID;
END;
$$;


ALTER FUNCTION pangya.procinsertmascot(_iduser integer, _itemtypeid integer, _tipo integer, _iscash integer, _tempo integer, _message text, _price integer) OWNER TO postgres;

--
-- TOC entry 736 (class 1255 OID 23423)
-- Name: procinsertmemorialrarewinlog(integer, integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procinsertmemorialrarewinlog(_iduser integer, _coin_typeid integer, _item_typeid integer, _item_qntd integer, _item_raridade integer, _item_probabilidade integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDUSER > 0 AND _COIN_TYPEID > 0 AND _ITEM_TYPEID > 0 THEN
		INSERT INTO pangya.pangya_new_memorial_rare_win_log(UID, coin_typeid, item_typeid, item_qntd, item_raridade, item_probabilidade, win_date)
			VALUES(_IDUSER, _COIN_TYPEID, _ITEM_TYPEID, _ITEM_QNTD, _ITEM_RARIDADE, _ITEM_PROBABILIDADE, now());
	END IF;
END;
$$;


ALTER FUNCTION pangya.procinsertmemorialrarewinlog(_iduser integer, _coin_typeid integer, _item_typeid integer, _item_qntd integer, _item_raridade integer, _item_probabilidade integer) OWNER TO postgres;

--
-- TOC entry 737 (class 1255 OID 23424)
-- Name: procinsertmemorialshopitemrarewin(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procinsertmemorialshopitemrarewin(_iduser integer, _itemtypeid integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	INSERT INTO pangya.memorialshopitemrarewin(pangya.memorialshopitemrarewin.UID, pangya.memorialshopitemrarewin.TYPEID, pangya.memorialshopitemrarewin.REG_DATE)
         VALUES (_IDUSER, _ITEMTYPEID, now());
END;
$$;


ALTER FUNCTION pangya.procinsertmemorialshopitemrarewin(_iduser integer, _itemtypeid integer) OWNER TO postgres;

--
-- TOC entry 738 (class 1255 OID 23425)
-- Name: procinsertnewachievement(integer, text, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procinsertnewachievement(_iduser integer, _nome text, _typeid integer, _active integer, _status integer) RETURNS TABLE("_ID_ACHIEVEMENT_" bigint)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_ID_ACHIEVEMENT bigint = -1;
BEGIN
	IF _TYPEID != 0 THEN
		INSERT INTO pangya.pangya_achievement(UID, NOME, TYPEID, ACTIVE, STATUS) VALUES(_IDUSER, _NOME, _TYPEID, /*_QUEST_TYPEID, _COUNTERITEM_ID,*/ _ACTIVE, _STATUS)
		RETURNING ID_ACHIEVEMENT INTO _ID_ACHIEVEMENT;
	END IF;
	
	RETURN QUERY SELECT _ID_ACHIEVEMENT AS ID_ACHIEVEMENT;
END;
$$;


ALTER FUNCTION pangya.procinsertnewachievement(_iduser integer, _nome text, _typeid integer, _active integer, _status integer) OWNER TO postgres;

--
-- TOC entry 739 (class 1255 OID 23426)
-- Name: procinsertnewquest(integer, text, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procinsertnewquest(_iduser integer, _nome text, _id_achievement integer, _quest_typeid integer, _counteritem_typeid integer) RETURNS TABLE("_ID_" bigint, "_COUNTER_ID_" bigint)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_ID bigint = -1;
	_COUNTERITEM_ID bigint = 0;
BEGIN
	IF _ID_ACHIEVEMENT != 0 AND _QUEST_TYPEID != 0 THEN

		IF _COUNTERITEM_TYPEID != 0 THEN
			INSERT INTO pangya.pangya_counter_item(UID, TYPEID, ACTIVE, COUNT_NUM_ITEM) VALUES(_IDUSER, _COUNTERITEM_TYPEID, 1, 0)
			RETURNING Count_ID INTO _COUNTERITEM_ID;
		END IF;

		INSERT INTO pangya.pangya_quest(ACHIEVEMENT_ID, UID, NAME, TYPEID, COUNTER_ITEM_ID, DATE) VALUES(_ID_ACHIEVEMENT, _IDUSER, _NOME, _QUEST_TYPEID, _COUNTERITEM_ID, NULL)
		RETURNING id INTO _ID;
	END IF;
	
	RETURN QUERY SELECT _ID AS ID, _COUNTERITEM_ID AS COUNTER_ITEM_ID;
END;
$$;


ALTER FUNCTION pangya.procinsertnewquest(_iduser integer, _nome text, _id_achievement integer, _quest_typeid integer, _counteritem_typeid integer) OWNER TO postgres;

--
-- TOC entry 740 (class 1255 OID 23427)
-- Name: procinsertnewticketreport(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procinsertnewticketreport(_typeid_trofel integer, _tipo_ticker_report integer) RETURNS TABLE("_TICKET_ID_" bigint)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_IDX bigint = -1;
BEGIN
	INSERT INTO pangya.pangya_ticket_report(trofel_typeid, flag, reg_date, tipo)
	VALUES (_typeid_trofel, 0, now(), _tipo_ticker_report)
	RETURNING idx INTO _IDX;

	RETURN QUERY SELECT _IDX AS ticket_id;
END;
$$;


ALTER FUNCTION pangya.procinsertnewticketreport(_typeid_trofel integer, _tipo_ticker_report integer) OWNER TO postgres;

--
-- TOC entry 741 (class 1255 OID 23428)
-- Name: procinsertpapelshoprarewinlog(integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procinsertpapelshoprarewinlog(_iduser integer, _item_typeid integer, _item_qntd integer, _color integer, _probabilidade integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDUSER > 0 THEN
		INSERT INTO pangya_papel_shop_rare_win_log(UID, typeid, qntd, ball_color, probabilidade)
			VALUES(_IDUSER, _ITEM_TYPEID, _ITEM_QNTD, _COLOR, _PROBABILIDADE);
	END IF;
END;
$$;


ALTER FUNCTION pangya.procinsertpapelshoprarewinlog(_iduser integer, _item_typeid integer, _item_qntd integer, _color integer, _probabilidade integer) OWNER TO postgres;

--
-- TOC entry 742 (class 1255 OID 23429)
-- Name: procinsertpersonalshoplog(integer, integer, integer, integer, integer, integer, bigint, bigint); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procinsertpersonalshoplog(_uid_sell integer, _uid_buy integer, _item_typeid integer, _item_id_sell integer, _item_id_buy integer, _item_qntd integer, _item_pang bigint, _total_pang bigint) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _UID_SELL > 0 AND _UID_BUY > 0 THEN
		INSERT INTO pangya.pangya_personal_shop_log(player_sell_uid, player_buy_uid, item_typeid, item_id_sell, item_id_buy, item_qntd, item_pang, total_pang, reg_date)
			VALUES(_UID_SELL, _UID_BUY, _ITEM_TYPEID, _ITEM_ID_SELL, _ITEM_ID_BUY, _ITEM_QNTD, _ITEM_PANG, _TOTAL_PANG, now());
	END IF;
END;
$$;


ALTER FUNCTION pangya.procinsertpersonalshoplog(_uid_sell integer, _uid_buy integer, _item_typeid integer, _item_id_sell integer, _item_id_buy integer, _item_qntd integer, _item_pang bigint, _total_pang bigint) OWNER TO postgres;

--
-- TOC entry 743 (class 1255 OID 23430)
-- Name: procinsertrecordplayercourse(integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, bigint, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procinsertrecordplayercourse(_iduser integer, _tipo_in integer, _course_in integer, _tacada_in integer, _putt_in integer, _hole_in integer, _fairway_in integer, _holein_in integer, _puttin_in integer, _best_score_in integer, _best_pang_in bigint, _character_typeid_in integer, _assist_in integer, _event_score_in integer, _flag integer) RETURNS TABLE("_RECORD_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_TipoRet int = 0;
BEGIN
	IF 
	(
		SELECT pangya_record.course
		FROM pangya.pangya_record
		WHERE 
			pangya_record.UID = _IDUSER AND 
			pangya_record.course = _course_in AND 
			pangya_record.tipo = _tipo_in AND 
			pangya_record.assist = _assist_in
	) IS NULL THEN
		INSERT INTO pangya.pangya_record(pangya.pangya_record.UID, pangya.pangya_record.course, pangya.pangya_record.tipo, pangya.pangya_record.assist)
		VALUES (_IDUSER, _course_in, _tipo_in, _assist_in);
	END IF;

	IF _flag = 1 THEN

		_TipoRet = 
		(
			SELECT 
				CASE 
					WHEN (pangya_record.best_score = 127 OR pangya_record.best_score > _best_score_in) OR pangya_record.best_pang < _best_pang_in THEN 1
					ELSE 0
				END
			FROM pangya.pangya_record
			WHERE 
				pangya_record.UID = _IDUSER AND 
				pangya_record.course = _course_in AND 
				pangya_record.tipo = _tipo_in AND 
				pangya_record.assist = _assist_in
		);

		IF _TipoRet = 1 THEN
			UPDATE pangya.user_info
				SET 
					pang = user_info.Pang + 1000
			WHERE user_info.UID = _IDUSER;
		END IF;

		UPDATE pangya.pangya_record
			SET 
				best_score = 
					CASE 
						WHEN (pangya_record.best_score = 127 OR pangya_record.best_score > _best_score_in) THEN _best_score_in
						ELSE pangya_record.best_score
					END, 
				best_pang = 
					CASE 
						WHEN pangya_record.best_pang < _best_pang_in THEN _best_pang_in
						ELSE pangya_record.best_pang
					END, 
				character_typeid = 
					CASE 
						WHEN _TipoRet = 1 THEN _character_typeid_in
						ELSE pangya_record.character_typeid
					END
		WHERE 
			pangya_record.UID = _IDUSER AND 
			pangya_record.course = _course_in AND 
			pangya_record.tipo = _tipo_in AND 
			pangya_record.assist = _assist_in;

	END IF;

	UPDATE pangya.pangya_record
		SET 
			tacada = pangya_record.tacada + _tacada_in, 
			putt = pangya_record.putt + _putt_in, 
			hole = pangya_record.hole + _hole_in, 
			fairway = pangya_record.fairway + _fairway_in, 
			holein = pangya_record.holein + _holein_in, 
			puttin = pangya_record.puttin + _puttin_in, 
			total_score = pangya_record.total_score + _best_score_in, 
			event_score = _event_score_in
	WHERE 
		pangya_record.UID = _IDUSER AND 
		pangya_record.course = _course_in AND 
		pangya_record.tipo = _tipo_in AND 
		pangya_record.assist = _assist_in;

	RETURN QUERY SELECT _TipoRet AS record;
END;
$$;


ALTER FUNCTION pangya.procinsertrecordplayercourse(_iduser integer, _tipo_in integer, _course_in integer, _tacada_in integer, _putt_in integer, _hole_in integer, _fairway_in integer, _holein_in integer, _puttin_in integer, _best_score_in integer, _best_pang_in bigint, _character_typeid_in integer, _assist_in integer, _event_score_in integer, _flag integer) OWNER TO postgres;

--
-- TOC entry 744 (class 1255 OID 23431)
-- Name: procinsertrewardtodonate(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procinsertrewardtodonate(_iduser integer, _cp integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
DECLARE
	_MSGID INT = 0;
BEGIN
	INSERT INTO pangya.pangya_gift_table(UID, fromid, message, giftdate, Lida_YN, valid, Flag, Contador_Vista)
		VALUES(_IDUSER, '@SUPERSS', CONCAT('Obrigado pela sua docao(donate), aqui vai uns premios para voce. E mais ', _CP, 'CP'),
			now(), 0, 1, 0, 0)
		RETURNING Msg_ID INTO _MSGID;

	IF _MSGID > 0 THEN

		perform pangya.ProcInsertItemNoEmail(0, _IDUSER, _MSGID, -1, 2080374803, 0, 1, 0);
		perform pangya.ProcInsertItemNoEmail(0, _IDUSER, _MSGID, -1, 2084569107, 0, 1, 0);
		perform pangya.ProcInsertItemNoEmail(0, _IDUSER, _MSGID, -1, 2084569113, 0, 1, 0);
		perform pangya.ProcInsertItemNoEmail(0, _IDUSER, _MSGID, -1, 1879113856, 0, 1, 0);
		perform pangya.ProcInsertItemNoEmail(0, _IDUSER, _MSGID, -1, 268435541, 0, 1, 0);

		perform pangya.ProcInsertCommand(4, _IDUSER, _MSGID, 0, 0, 0, 1, 0, 1, null);

		UPDATE pangya.user_info SET Cookie = Cookie + _CP WHERE UID = _IDUSER;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procinsertrewardtodonate(_iduser integer, _cp integer) OWNER TO postgres;

--
-- TOC entry 745 (class 1255 OID 23432)
-- Name: procinsertskin(integer, integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procinsertskin(_iduser integer, _itemtypeid integer, _flag integer, _giftflag integer, _purchase integer, _tempo integer) RETURNS TABLE("_ITEM_ID_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_ITEM_ID INT = -1;
BEGIN
	IF _ITEMTYPEID > 0 AND (SELECT UID FROM pangya.account WHERE UID = _IDUSER) IS NOT NULL THEN
	DECLARE
		_regDT timestamp = now();
		_endDT timestamp = _regDT;
	BEGIN
		_ITEM_ID = COALESCE((SELECT item_id FROM pangya.pangya_item_warehouse WHERE UID = _IDUSER AND typeid = _ITEMTYPEID AND valid = 1), -1);

		IF _ITEM_ID > 0 AND _ITEM_ID IS NOT NULL THEN
		DECLARE
			_diff INT = 0;
		BEGIN

			SELECT (case when DATEDIFF(SECOND, getdate(), EndDate) > 0 THEN 1 ELSE 0 END) INTO _diff FROM pangya.pangya_item_warehouse WHERE UID = _IDUSER AND item_id = _ITEM_ID;

			UPDATE pangya.pangya_item_warehouse SET EndDate = ((CASE WHEN _diff = 1 THEN EndDate ELSE now()::timestamp END) + ('1 day'::interval * _TEMPO)), C3 = C3 + _TEMPO
			WHERE UID = _IDUSER AND item_id = _ITEM_ID;
		END;
		ELSE
			IF _TEMPO > 0 THEN
				_endDT = (now() + ('1 day'::interval * _TEMPO));
			END IF;

			INSERT INTO pangya_item_warehouse(UID, typeid, valid, flag, Gift_flag, Purchase, ItemType, c3, regdate, EndDate)
				VALUES(_IDUSER, _ITEMTYPEID, 1, _FLAG, _GIFTFLAG, _PURCHASE, 2, _TEMPO, _regDT, _endDT)
				RETURNING item_id INTO _ITEM_ID;
		END IF;
	END;
	END IF;

	RETURN QUERY SELECT _ITEM_ID AS ITEM_ID;
END;
$$;


ALTER FUNCTION pangya.procinsertskin(_iduser integer, _itemtypeid integer, _flag integer, _giftflag integer, _purchase integer, _tempo integer) OWNER TO postgres;

--
-- TOC entry 746 (class 1255 OID 23433)
-- Name: procinsertspinningcubesuperrarewinbroadcast(text, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procinsertspinningcubesuperrarewinbroadcast(_msg text, _opt integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
DECLARE
	_NOTICE_ID bigint;
BEGIN
	INSERT INTO pangya.pangya_notice_list(message) VALUES(_MSG)
	RETURNING notice_id INTO _NOTICE_ID;
	
	INSERT INTO pangya.pangya_command(command_id, arg1, arg2, target)
        VALUES (2, _NOTICE_ID, _OPT, 1);
END;
$$;


ALTER FUNCTION pangya.procinsertspinningcubesuperrarewinbroadcast(_msg text, _opt integer) OWNER TO postgres;

--
-- TOC entry 747 (class 1255 OID 23434)
-- Name: procinsertticketreportdados(integer, integer, integer, integer, integer, bigint, bigint, integer, integer, integer, integer, integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procinsertticketreportdados(_ticker_id integer, _player_uid_in integer, _player_score_in integer, _player_medalha_in integer, _player_trofel_in integer, _player_pang_in bigint, _player_bonus_pang_in bigint, _player_exp_in integer, _player_mascot_typeid_in integer, _flag_item_pang_in integer, _flag_premium_user_in integer, _player_state_in integer, _player_finish_date text) RETURNS void
    LANGUAGE plpgsql
    AS $$
DECLARE
	_NOTICE_ID bigint;
BEGIN
	INSERT INTO pangya.pangya_ticket_report_dados(report_id, player_uid, player_score, player_medalha, player_trofel, player_pang, 
         player_bonus_pang, player_exp, player_mascot_typeid, player_state, flag_item_pang, flag_premium_user, finish_date)
      VALUES (_ticker_id, _player_uid_in, _player_score_in, _player_medalha_in, _player_trofel_in, _player_pang_in, _player_bonus_pang_in, 
            _player_exp_in, _player_mascot_typeid_in, _player_state_in, _flag_item_pang_in, _flag_premium_user_in, _player_finish_date::timestamp);
END;
$$;


ALTER FUNCTION pangya.procinsertticketreportdados(_ticker_id integer, _player_uid_in integer, _player_score_in integer, _player_medalha_in integer, _player_trofel_in integer, _player_pang_in bigint, _player_bonus_pang_in bigint, _player_exp_in integer, _player_mascot_typeid_in integer, _flag_item_pang_in integer, _flag_premium_user_in integer, _player_state_in integer, _player_finish_date text) OWNER TO postgres;

--
-- TOC entry 748 (class 1255 OID 23435)
-- Name: procmakeemailkey(text, text, text, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procmakeemailkey(_nome text, _sobrenome text, _email text, _ip text) RETURNS TABLE("_INDEX_" bigint, "_KEY_UNIQ_" uuid)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _NOME IS NOT NULL AND _SOBRENOME IS NOT NULL AND _EMAIL IS NOT NULL THEN
	DECLARE
		_id BIGINT;
	BEGIN

		INSERT INTO pangya.contas_beta(nome, sobre_nome, email, ip_register) VALUES(_NOME, _SOBRENOME, _EMAIL, _IP)
		RETURNING index INTO _id;

		RETURN QUERY SELECT index, key_uniq FROM pangya.contas_beta WHERE index = _id;
	END;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procmakeemailkey(_nome text, _sobrenome text, _email text, _ip text) OWNER TO postgres;

--
-- TOC entry 751 (class 1255 OID 23436)
-- Name: procmakeguild(text, integer, integer, text, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procmakeguild(_name text, _master integer, _condition_level integer, _intro text, _master_comment text) RETURNS TABLE("_GUILD_UID_" bigint, "_GUILD_NEW_MARK_IDX_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_GUILD_UID bigint = -1;
	_GUILD_NEW_MARK_IDX int = 1;
BEGIN
	IF _NAME != '' AND _MASTER != 0 THEN
	DECLARE
		_OTHER_GUILD INT = (
			SELECT
				a.Guild_UID
			FROM
				pangya.account a
				INNER JOIN
				pangya.pangya_guild b
				ON a.Guild_UID = b.GUILD_UID
			WHERE a.UID = _MASTER AND 
				(b.GUILD_STATE NOT IN(4, 5) OR b.GUILD_CLOSURE_DATE IS NULL OR now() < b.GUILD_CLOSURE_DATE)
		);
	BEGIN

		IF _OTHER_GUILD IS NULL THEN

			IF DATEDIFF('hour', COALESCE((SELECT MAX(REG_DATE) FROM pangya.pangya_guild_atividade_player WHERE UID = _MASTER AND FLAG = 8/*Create*/), (now() + '-1 day'::interval))::timestamp, now()::timestamp) >= 24/*1 Dia*/ THEN
			DECLARE
				_PANG BIGINT = (SELECT pang FROM pangya.user_info WHERE UID = _MASTER);
			BEGIN
		
				-- Verifica se o player tem os 5k de pangs para criar a guild
				IF _PANG IS NOT NULL AND _PANG >= 5000 THEN

					-- Delete ele de qualquer membro de guild que ele esteja, que o OTHER_GUILD é null, se ele estava em uma guild, ela já está fechada
					DELETE FROM pangya.pangya_guild_member WHERE MEMBER_UID = _MASTER;

					-- Cria a Guild
					INSERT INTO pangya.pangya_guild(GUILD_NAME, GUILD_LEADER, GUILD_CONDITION_LEVEL, GUILD_INFO, GUILD_NOTICE, GUILD_NEW_MARK_IDX) 
					VALUES(_NAME, _MASTER, _CONDITION_LEVEL, _INTRO, _MASTER_COMMENT, 1)
					RETURNING GUILD_UID, GUILD_NEW_MARK_IDX INTO _GUILD_UID, _GUILD_NEW_MARK_IDX;

					-- Insert Member of guild
					INSERT INTO pangya.pangya_guild_member(GUILD_UID, MEMBER_UID, MEMBER_STATE_FLAG) VALUES(_GUILD_UID, _MASTER, 1/*Master*/);

					-- Update player account  GUILD_UID
					UPDATE
						pangya.account
							SET Guild_UID = _GUILD_UID
					WHERE UID = _MASTER;

					-- Insert Log Activety of player guild
					INSERT INTO pangya.pangya_guild_atividade_player(GUILD_UID, UID, FLAG) VALUES(_GUILD_UID, _MASTER, 8/*Criou uma Guild*/);

					-- Tira os 5k de pang que gastou para criar a guild
					UPDATE
						pangya.user_info
							SET pang = _PANG - 5000
					WHERE UID = _MASTER;

					-- Sucesso

				ELSE
					_GUILD_UID = -3;	-- Não tem pangs suficiente para criar a guild
				END IF;

			END;
			ELSE
				_GUILD_UID = -2;	-- Não pode criar a guild, faz menos de 24 horas que ele criou outra guild
			END IF;

		ELSE
			_GUILD_UID = -4;	-- Player já é membro de outra guild, saia dela para poder criar outra guild
		END IF;

	END;
	END IF;

	RETURN QUERY SELECT _GUILD_UID AS GUILD_UID, _GUILD_NEW_MARK_IDX AS GUILD_NEW_MARK_IDX;
END;
$$;


ALTER FUNCTION pangya.procmakeguild(_name text, _master integer, _condition_level integer, _intro text, _master_comment text) OWNER TO postgres;

--
-- TOC entry 752 (class 1255 OID 23437)
-- Name: procmakeuserbeta(text, text, text, text, text, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procmakeuserbeta(_nome_in text, _sobre_nome_in text, _email_in text, _id_in text, _pass_in text, _ip_in text) RETURNS TABLE("_UID_" bigint)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_IDUSER bigint = 0;
BEGIN
	SELECT "_UID" INTO _IDUSER FROM pangya.ProcNewUser(_id_in, _pass_in, _ip_in, 0);

	INSERT INTO pangya.contas_beta(uid, nome, sobre_nome, email)
	VALUES (_IDUSER, _nome_in, _sobre_nome_in, _email_in);

	INSERT INTO pangya.pangya_player_ip(uid, ip)
	VALUES (_IDUSER, _ip_in);
	
	RETURN QUERY SELECT _IDUSER AS UID;
END;
$$;


ALTER FUNCTION pangya.procmakeuserbeta(_nome_in text, _sobre_nome_in text, _email_in text, _id_in text, _pass_in text, _ip_in text) OWNER TO postgres;

--
-- TOC entry 753 (class 1255 OID 23438)
-- Name: procmakeuserbetawithmd5(text, text, bigint, integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procmakeuserbetawithmd5(_id_in text, _pass_in text, _index bigint, _sex integer, _ip_in text) RETURNS TABLE("_UID_" bigint)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_IDUSER bigint = -1;
BEGIN
	IF (SELECT key_uniq FROM pangya.contas_beta WHERE index = _index AND finish_reg = 0) IS NOT NULL THEN

		SELECT "_UID" INTO _IDUSER FROM pangya.ProcNewUserWithMD5(_id_in, _pass_in, _sex, _ip_in, 0);

		UPDATE pangya.contas_beta SET UID = _IDUSER, ip_register = _ip_in, finish_reg = 1 WHERE index = _index;

		INSERT INTO pangya.pangya_player_ip(uid, ip)
			VALUES (_IDUSER, _ip_in);
	END IF;
	
	RETURN QUERY SELECT _IDUSER AS UID;
END;
$$;


ALTER FUNCTION pangya.procmakeuserbetawithmd5(_id_in text, _pass_in text, _index bigint, _sex integer, _ip_in text) OWNER TO postgres;

--
-- TOC entry 754 (class 1255 OID 23439)
-- Name: procmoveitemdolfinilocker(integer, bigint); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procmoveitemdolfinilocker(_iduser integer, _idx_locker bigint) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	UPDATE pangya.pangya_item_warehouse a
		SET valid = 1
	FROM pangya.pangya_dolfini_locker_item b
	WHERE a.UID = _IDUSER AND a.item_id = b.item_id AND b.idx = _IDX_LOCKER;

	UPDATE pangya.pangya_dolfini_locker_item SET flag = 0 WHERE UID = _IDUSER AND idx = _IDX_LOCKER;
END;
$$;


ALTER FUNCTION pangya.procmoveitemdolfinilocker(_iduser integer, _idx_locker bigint) OWNER TO postgres;

--
-- TOC entry 755 (class 1255 OID 23440)
-- Name: procmoveitemfrommailtoarmario(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procmoveitemfrommailtoarmario(_iduser integer, _idmsg integer) RETURNS TABLE("_ITEM_ID_" integer, "_TYPEID_" integer, "_QUANTIDADE_" integer, "_QNTD_DIA_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT pangya_item_mail.item_id AS ITEM_ID, pangya_item_mail.item_typeid AS TYPEID, pangya_item_mail.Quantidade_item AS QUANTIDADE, pangya_item_mail.Quantidade_Dia AS QNTD_DIA
      FROM pangya.pangya_item_mail
      WHERE pangya_item_mail.Msg_ID = _IDMSG AND pangya_item_mail.valid = 1;

      UPDATE pangya.pangya_gift_table
         SET 
            Lida_YN = 1
      WHERE pangya_gift_table.UID = _IDUSER AND pangya_gift_table.Msg_ID = _IDMSG;

      UPDATE pangya.pangya_item_mail
         SET 
            valid = 0
      WHERE pangya_item_mail.Msg_ID = _IDMSG;
END;
$$;


ALTER FUNCTION pangya.procmoveitemfrommailtoarmario(_iduser integer, _idmsg integer) OWNER TO postgres;

--
-- TOC entry 756 (class 1255 OID 23441)
-- Name: procnewuser(text, text, text, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procnewuser(_userid text, _pass text, _ipaddr text, _serveruid integer) RETURNS TABLE("_UID" bigint)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_IDUSER bigint DEFAULT 0;
	_PASSWORD_MD5 VARCHAR(32);
BEGIN
	
	_PASSWORD_MD5 = upper(md5(_pass));
	
	INSERT INTO pangya.account(ID, PASSWORD, LastLogonTime, RegDate, NICK, UserName, UserIp, ServerID, LogonCount)
	VALUES(_userID, _PASSWORD_MD5, now(), now(), _userID, _userID, _IPaddr, _serverUID, 1)
    RETURNING uid INTO _IDUSER; --#novo e melhor modo de pegar o uid do player adicionado
    --#SELECT UID INTO IDUSER FROM account WHERE ID = _userID;
    
    INSERT INTO pangya.user_info(UID) VALUES(_IDUSER);

	INSERT INTO pangya.pangya_player_location(UID) VALUES(_IDUSER);

	INSERT INTO pangya.pangya_user_equip(UID) VALUES(_IDUSER);

	INSERT INTO pangya.pangya_last_players_user(UID) VALUES(_IDUSER);

	INSERT INTO pangya.trofel_stat(UID) VALUES(_IDUSER);

	INSERT INTO pangya.tutorial(UID) VALUES(_IDUSER);

	INSERT INTO pangya.pangya_user_macro(UID) VALUES(_IDUSER);

	INSERT INTO pangya.pangya_attendance_reward(UID) VALUES(_IDUSER);

	INSERT INTO pangya.pangya_scratchy_prob_sec(UID) VALUES(_IDUSER);

	INSERT INTO pangya.black_papel_prob_sec(UID) VALUES(_IDUSER);

	INSERT INTO pangya.pangya_tiki_points(UID) VALUES(_IDUSER);

	INSERT INTO pangya.pangya_myroom(UID) VALUES(_IDUSER);

	INSERT INTO pangya.pangya_dolfini_locker(UID) VALUES(_IDUSER);
    
    INSERT INTO pangya.pangya_assistente(UID) VALUES(_IDUSER);
    
    INSERT INTO pangya.pangya_grand_zodiac_pontos(UID) VALUES(_IDUSER);
    
    INSERT INTO pangya.pangya_daily_quest_player(UID, LAST_QUEST_ACCEPT, TODAY_QUEST) VALUES(_IDUSER, NULL, NULL);

	INSERT INTO pangya.pangya_papel_shop_info(UID, remain_cnt, limit_cnt) VALUES(_IDUSER, 100, 100);

	INSERT INTO pangya.td_room_data(UID, TYPEID, POS_X, POS_Y, POS_Z, POS_R)
	VALUES(_IDUSER, 1207986208, 15.2, 0, 12.5, 152);
	
	INSERT INTO pangya.td_room_data(UID, TYPEID, POS_X, POS_Y, POS_Z, POS_R)
	VALUES(_IDUSER, 1207980061, 14.125, 1.0, 0.041, 0);
	
	-- Cria Achievement Dados do usuario Não mais
	--exec pangya.USP_CreateAchievementUser _IDUSER;

	RETURN QUERY SELECT _IDUSER as UID;
END;
$$;


ALTER FUNCTION pangya.procnewuser(_userid text, _pass text, _ipaddr text, _serveruid integer) OWNER TO postgres;

--
-- TOC entry 757 (class 1255 OID 23442)
-- Name: procnewuserwithmd5(text, text, integer, text, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procnewuserwithmd5(_userid text, _pass text, _sex integer, _ipaddr text, _serveruid integer) RETURNS TABLE("_UID" bigint)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_IDUSER BIGINT = 0;
BEGIN
	INSERT INTO pangya.account(ID, PASSWORD, Sex, LastLogonTime, RegDate, NICK, UserName, UserIp, ServerID, LogonCount)
	VALUES(_userID, _pass, _sex, now(), now(), _userID, _userID, _IPaddr, _serverUID, 1)
	RETURNING uid INTO _IDUSER;
    
    INSERT INTO pangya.user_info(UID) VALUES(_IDUSER);

	INSERT INTO pangya.pangya_player_location(UID) VALUES(_IDUSER);

	INSERT INTO pangya.pangya_user_equip(UID) VALUES(_IDUSER);

	INSERT INTO pangya.pangya_last_players_user(UID) VALUES(_IDUSER);

	INSERT INTO pangya.trofel_stat(UID) VALUES(_IDUSER);

	INSERT INTO pangya.tutorial(UID) VALUES(_IDUSER);

	INSERT INTO pangya.pangya_user_macro(UID) VALUES(_IDUSER);

	INSERT INTO pangya.pangya_attendance_reward(UID) VALUES(_IDUSER);

	INSERT INTO pangya.pangya_scratchy_prob_sec(UID) VALUES(_IDUSER);

	INSERT INTO pangya.black_papel_prob_sec(UID) VALUES(_IDUSER);

	INSERT INTO pangya.pangya_tiki_points(UID) VALUES(_IDUSER);

	INSERT INTO pangya.pangya_myroom(UID) VALUES(_IDUSER);

	INSERT INTO pangya.pangya_dolfini_locker(UID) VALUES(_IDUSER);
    
    INSERT INTO pangya.pangya_assistente(UID) VALUES(_IDUSER);
    
    INSERT INTO pangya.pangya_grand_zodiac_pontos(UID) VALUES(_IDUSER);
    
    INSERT INTO pangya.pangya_daily_quest_player(UID, LAST_QUEST_ACCEPT, TODAY_QUEST) VALUES(_IDUSER, NULL, NULL);

	INSERT INTO pangya.pangya_papel_shop_info(UID, remain_cnt, limit_cnt) VALUES(_IDUSER, 100, 100);

	INSERT INTO pangya.td_room_data(UID, TYPEID, POS_X, POS_Y, POS_Z, POS_R)
	VALUES(_IDUSER, 1207986208, 15.2, 0, 12.5, 152);
	
	INSERT INTO pangya.td_room_data(UID, TYPEID, POS_X, POS_Y, POS_Z, POS_R)
	VALUES(_IDUSER, 1207980061, 14.125, 1.0, 0.041, 0);
	
	-- Cria Achievement Dados do usuario Não mais
	--exec pangya.USP_CreateAchievementUser @IDUSER;

	RETURN QUERY SELECT _IDUSER as UID;
END;
$$;


ALTER FUNCTION pangya.procnewuserwithmd5(_userid text, _pass text, _sex integer, _ipaddr text, _serveruid integer) OWNER TO postgres;

--
-- TOC entry 749 (class 1255 OID 23443)
-- Name: procpedeparaentraguild(integer, integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procpedeparaentraguild(_iduser integer, _uid_guild integer, _msg text) RETURNS TABLE("_GUILD_UID_" bigint)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_GUILD_UID BIGINT = -1;
BEGIN
	IF (1 <> 1) THEN

		INSERT INTO pangya.pangya_guild_member(
		   pangya.pangya_guild_member.GUILD_UID, 
		   pangya.pangya_guild_member.MEMBER_UID, 
		   pangya.pangya_guild_member.MEMBER_MSG, 
		   pangya.pangya_guild_member.MEMBER_STATE_FLAG, 
		   pangya.pangya_guild_member.GUILD_PANG, 
		   pangya.pangya_guild_member.GUILD_POINT)
		   VALUES (
			  _UID_GUILD, 
			  _IDUSER, 
			  _MSG, 
			  9, 
			  0, 
			  0);

		INSERT INTO pangya.pangya_guild_atividade_player(pangya.pangya_guild_atividade_player.UID, pangya.pangya_guild_atividade_player.GUILD_UID, pangya.pangya_guild_atividade_player.FLAG)
		   VALUES (_IDUSER, _UID_GUILD, 1);

		UPDATE pangya.account
		   SET 
			  GUILD_UID = _UID_GUILD
		WHERE account.UID = _IDUSER;

	 END IF;

	RETURN QUERY SELECT _UID_GUILD AS GUILD_UID;
END;
$$;


ALTER FUNCTION pangya.procpedeparaentraguild(_iduser integer, _uid_guild integer, _msg text) OWNER TO postgres;

--
-- TOC entry 750 (class 1255 OID 23444)
-- Name: procrecoveryclubset(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procrecoveryclubset(_iduser integer, _taqueira_id integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	UPDATE pangya.pangya_item_warehouse
         SET 
            Recovery_Pts = 0
      WHERE pangya_item_warehouse.UID = _IDUSER AND pangya_item_warehouse.item_id = _taqueira_id;
END;
$$;


ALTER FUNCTION pangya.procrecoveryclubset(_iduser integer, _taqueira_id integer) OWNER TO postgres;

--
-- TOC entry 759 (class 1255 OID 23445)
-- Name: procregisterdonation(integer, integer, integer, integer, text, text, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procregisterdonation(_adm_uid integer, _iduser integer, _plataforma integer, _cash integer, _email text, _obs text, _item_typeid_1 integer, _item_typeid_2 integer, _item_typeid_3 integer, _item_typeid_4 integer, _item_typeid_5 integer, _item_qntd_1 integer, _item_qntd_2 integer, _item_qntd_3 integer, _item_qntd_4 integer, _item_qntd_5 integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDUSER > 0 AND _CASH > 0 THEN
	DECLARE
		_CP INT = _CASH * 10;
		_DONATION_ID INT;
		_MSGID INT;
	BEGIN

		INSERT INTO pangya.pangya_donation_log(ADM_UID, UID, plataforma, cash, cookie_point, email, obs)
			VALUES(_ADM_UID, _IDUSER, _PLATAFORMA, _CASH, _CP, _EMAIL, _OBS)
			RETURNING index INTO _DONATION_ID;

		-- UPDATE COOKIE POINT
		UPDATE pangya.user_info SET Cookie = Cookie + _CP WHERE UID = _IDUSER;

		IF (_ITEM_TYPEID_1 != 0 AND _ITEM_QNTD_1 > 0) 
			OR (_ITEM_TYPEID_2 != 0 AND _ITEM_QNTD_2 > 0) 
			OR (_ITEM_TYPEID_3 != 0 AND _ITEM_QNTD_3 > 0) 
			OR (_ITEM_TYPEID_4 != 0 AND _ITEM_QNTD_4 > 0) 
			OR (_ITEM_TYPEID_5 != 0 AND _ITEM_QNTD_5 > 0) THEN
			
			INSERT INTO pangya.pangya_gift_table(UID, fromid, message, giftdate, Lida_YN, valid, Flag, Contador_Vista)
			VALUES(_IDUSER, '@SUPERSS', CONCAT(E'\cFF09A02A,t\cObrigado pela sua docao(donate), aqui vai uns premios para voce. E mais ', _CP, 'CP'),
				now(), 0, 1, 0, 0)
			RETURNING Msg_ID INTO _MSGID;

			IF _MSGID > 0 THEN

				IF (_ITEM_TYPEID_1 != 0 AND _ITEM_QNTD_1 > 0) THEN
					INSERT INTO pangya.pangya_donation_item_log(donation_id, item_typeid, item_qntd) VALUES(_DONATION_ID, _ITEM_TYPEID_1, _ITEM_QNTD_1);

					perform pangya.ProcInsertItemNoEmail(0, _IDUSER, _MSGID, -1, _ITEM_TYPEID_1, 0, _ITEM_QNTD_1, 0);
				END IF;

				IF (_ITEM_TYPEID_2 != 0 AND _ITEM_QNTD_2 > 0) THEN
					INSERT INTO pangya.pangya_donation_item_log(donation_id, item_typeid, item_qntd) VALUES(_DONATION_ID, _ITEM_TYPEID_2, _ITEM_QNTD_2);

					perform pangya.ProcInsertItemNoEmail(0, _IDUSER, _MSGID, -1, _ITEM_TYPEID_2, 0, _ITEM_QNTD_2, 0);
				END IF;

				IF (_ITEM_TYPEID_3 != 0 AND _ITEM_QNTD_3 > 0) THEN
					INSERT INTO pangya.pangya_donation_item_log(donation_id, item_typeid, item_qntd) VALUES(_DONATION_ID, _ITEM_TYPEID_3, _ITEM_QNTD_3);

					perform pangya.ProcInsertItemNoEmail(0, _IDUSER, _MSGID, -1, _ITEM_TYPEID_3, 0, _ITEM_QNTD_3, 0);
				END IF;

				IF (_ITEM_TYPEID_4 != 0 AND _ITEM_QNTD_4 > 0) THEN
					INSERT INTO pangya.pangya_donation_item_log(donation_id, item_typeid, item_qntd) VALUES(_DONATION_ID, _ITEM_TYPEID_4, _ITEM_QNTD_4);

					perform pangya.ProcInsertItemNoEmail(0, _IDUSER, _MSGID, -1, _ITEM_TYPEID_4, 0, _ITEM_QNTD_4, 0);
				END IF;

				IF (_ITEM_TYPEID_5 != 0 AND _ITEM_QNTD_5 > 0) THEN
					INSERT INTO pangya.pangya_donation_item_log(donation_id, item_typeid, item_qntd) VALUES(_DONATION_ID, _ITEM_TYPEID_5, _ITEM_QNTD_5);

					perform pangya.ProcInsertItemNoEmail(0, _IDUSER, _MSGID, -1, _ITEM_TYPEID_5, 0, _ITEM_QNTD_5, 0);
				END IF;

				-- Comando para mostrar no jogo que o player recebeu um presente
				perform pangya.ProcInsertCommand(4, _IDUSER, _MSGID, 0, 0, 0, 1, 0, 1, null);

			END IF; -- IF MSG IS IS OK
		
		-- IF CHECK ITENS IS OK TO SEND
		ELSE
			
			-- Não tem itens para enviar só invia uma mensagem
			INSERT INTO pangya.pangya_gift_table(UID, fromid, message, giftdate, Lida_YN, valid, Flag, Contador_Vista)
			VALUES(_IDUSER, '@SUPERSS', CONCAT(E'\cFF09A02A,t\cObrigado pela sua docao(donate) foi adiconado a sua conta ', _CP, 'CP'),
				now(), 0, 1, 0, 0)
			RETURNING Msg_ID INTO _MSGID;

			IF _MSGID > 0 THEN
				-- Comando para mostrar no jogo que o player recebeu um presente
				perform pangya.ProcInsertCommand(4, _IDUSER, _MSGID, 0, 0, 0, 1, 0, 1, null);
			END IF;

		END IF;	-- ELSE CHECK ITENS IS OK TO SEND

	END; -- IFF CHECK UID AND CASH
	END IF;
END;
$$;


ALTER FUNCTION pangya.procregisterdonation(_adm_uid integer, _iduser integer, _plataforma integer, _cash integer, _email text, _obs text, _item_typeid_1 integer, _item_typeid_2 integer, _item_typeid_3 integer, _item_typeid_4 integer, _item_typeid_5 integer, _item_qntd_1 integer, _item_qntd_2 integer, _item_qntd_3 integer, _item_qntd_4 integer, _item_qntd_5 integer) OWNER TO postgres;

--
-- TOC entry 760 (class 1255 OID 23446)
-- Name: procregisterdonationepin(bigint, integer, bigint); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procregisterdonationepin(_donation_id bigint, _uid integer, _value bigint) RETURNS TABLE("_ID_" bigint, "_EPIN_" uuid, "_EMAIL_" character varying)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_ID BIGINT = -1;
BEGIN
	IF _DONATION_ID > 0 AND _UID > 0 THEN

		INSERT INTO pangya.pangya_donation_epin(donation_id, uid, qntd, valid)
		VALUES(_DONATION_ID, _UID, _VALUE, 1)
		RETURNING index INTO _ID;

		RETURN QUERY SELECT 
			a.index AS ID, 
			a.epin,
			b.email
		FROM
			pangya.pangya_donation_epin a
			INNER JOIN
			pangya.contas_beta b
			ON a.UID = b.UID
		WHERE
			a.index = _ID;
	ELSE
		RETURN QUERY SELECT -1::bigint AS ID,
							null::uuid,
							null::varchar(100);
	END IF;
END;
$$;


ALTER FUNCTION pangya.procregisterdonationepin(_donation_id bigint, _uid integer, _value bigint) OWNER TO postgres;

--
-- TOC entry 761 (class 1255 OID 23447)
-- Name: procregistergachajpplayerwin(integer, integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procregistergachajpplayerwin(_uid integer, _count_item integer, _str text) RETURNS TABLE("_FIRST_ID_" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _UID > 0 AND _COUNT_ITEM > 0 AND _STR IS NOT NULL AND _STR != '' THEN
	DECLARE
		_FIRSTID BIGINT = -1;
		_ID BIGINT = -1;
		_ITEM VARCHAR(50);
		_ITEM_TYPEID INT;
		_ITEM_QNTD INT;
		_ITEM_RARITY INT;
		_ITEM_GACHA_NUM INT;
		_ITEM_COUNT INT = 0;
		_ITEM_ID INT = -1;
		-- Cursor
		itens CURSOR FOR
			SELECT regexp_split_to_table as Item FROM regexp_split_to_table(_STR, '\|');
	BEGIN

		OPEN itens;
		FETCH NEXT FROM itens INTO _ITEM;
		WHILE found LOOP

			IF _ITEM IS NOT NULL THEN
				
				_ITEM_TYPEID = split_part(_ITEM, ';', 1);
				_ITEM_QNTD = split_part(_ITEM, ';', 2);
				_ITEM_RARITY = split_part(_ITEM, ';', 3);
				_ITEM_GACHA_NUM = split_part(_ITEM, ';', 4);

				IF _ITEM_TYPEID IS NOT NULL AND _ITEM_QNTD IS NOT NULL AND _ITEM_RARITY IS NOT NULL AND _ITEM_GACHA_NUM IS NOT NULL THEN
							
					INSERT INTO pangya.pangya_gacha_jp_player_win(UID, typeid, qnty, rarity_type, gacha_num, valid, send_mail)
					VALUES(_UID, _ITEM_TYPEID, _ITEM_QNTD, _ITEM_RARITY, _ITEM_GACHA_NUM, 0, 0)
					RETURNING index INTO _ID; -- Ativo depois se der tudo certo
					
					IF _FIRSTID = -1 THEN
						_FIRSTID = _ID;
					END IF;

					-- Itens inseridos no mail
					_ITEM_COUNT = _ITEM_COUNT + 1;
				END IF;
			END IF;

			FETCH NEXT FROM itens INTO _ITEM;

		END LOOP;

		-- CLOSE AND DEALLOCATE CURSOR
		CLOSE itens;

		-- Verifica se foi os itens tudo OK
		IF _COUNT_ITEM = _ITEM_COUNT AND _COUNT_ITEM = (SELECT COUNT(index) FROM pangya.pangya_gacha_jp_player_win WHERE UID = _UID AND index >= _FIRSTID) THEN
			
			-- Tudo ok adiciona libera os itens
			UPDATE pangya.pangya_gacha_jp_player_win SET valid = 1 WHERE UID = _UID AND valid = 0 AND index >= _FIRSTID;

			-- Retorna o primeiro ID do ITEM
			RETURN QUERY SELECT _FIRSTID AS FIRST_ID;
		ELSE

			-- Deleta todos os que adicionou que não conseguiu adicionar o itens do player
			DELETE FROM pangya.pangya_gacha_jp_player_win 
			WHERE UID = _UID AND valid = 0 AND index >= _FIRSTID;
		END IF;
	END;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procregistergachajpplayerwin(_uid integer, _count_item integer, _str text) OWNER TO postgres;

--
-- TOC entry 762 (class 1255 OID 23448)
-- Name: procregistergmgiftweb(integer, integer, text, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procregistergmgiftweb(_adm_uid integer, _iduser integer, _msg text, _item_typeid_1 integer, _item_typeid_2 integer, _item_typeid_3 integer, _item_typeid_4 integer, _item_typeid_5 integer, _item_qntd_1 integer, _item_qntd_2 integer, _item_qntd_3 integer, _item_qntd_4 integer, _item_qntd_5 integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDUSER > 0 THEN
	DECLARE
		_MSGID INT;
	BEGIN

		IF (_ITEM_TYPEID_1 != 0 AND _ITEM_QNTD_1 > 0) 
			OR (_ITEM_TYPEID_2 != 0 AND _ITEM_QNTD_2 > 0) 
			OR (_ITEM_TYPEID_3 != 0 AND _ITEM_QNTD_3 > 0) 
			OR (_ITEM_TYPEID_4 != 0 AND _ITEM_QNTD_4 > 0) 
			OR (_ITEM_TYPEID_5 != 0 AND _ITEM_QNTD_5 > 0) THEN
			
			INSERT INTO pangya.pangya_gift_table(UID, fromid, message, giftdate, Lida_YN, valid, Flag, Contador_Vista)
			VALUES(_IDUSER, '@SUPERSS', _MSG, now(), 0, 1, 0, 0)
			RETURNING Msg_ID INTO _MSGID;

			IF _MSGID > 0 THEN

				-- Log
				INSERT INTO pangya.pangya_gm_gift_web_log(GM_UID, PLAYER_UID, MSG_ID) VALUES(_ADM_UID, _IDUSER, _MSGID);

				IF (_ITEM_TYPEID_1 != 0 AND _ITEM_QNTD_1 > 0) THEN
					perform pangya.ProcInsertItemNoEmail(0, _IDUSER, _MSGID, -1, _ITEM_TYPEID_1, 0, _ITEM_QNTD_1, 0);
				END IF;

				IF (_ITEM_TYPEID_2 != 0 AND _ITEM_QNTD_2 > 0) THEN
					perform pangya.ProcInsertItemNoEmail(0, _IDUSER, _MSGID, -1, _ITEM_TYPEID_2, 0, _ITEM_QNTD_2, 0);
				END IF;

				IF (_ITEM_TYPEID_3 != 0 AND _ITEM_QNTD_3 > 0) THEN
					perform pangya.ProcInsertItemNoEmail(0, _IDUSER, _MSGID, -1, _ITEM_TYPEID_3, 0, _ITEM_QNTD_3, 0);
				END IF;

				IF (_ITEM_TYPEID_4 != 0 AND _ITEM_QNTD_4 > 0) THEN
					perform pangya.ProcInsertItemNoEmail(0, _IDUSER, _MSGID, -1, _ITEM_TYPEID_4, 0, _ITEM_QNTD_4, 0);
				END IF;

				IF (_ITEM_TYPEID_5 != 0 AND _ITEM_QNTD_5 > 0) THEN
					perform pangya.ProcInsertItemNoEmail(0, _IDUSER, _MSGID, -1, _ITEM_TYPEID_5, 0, _ITEM_QNTD_5, 0);
				END IF;

				-- Comando para mostrar no jogo que o player recebeu um presente
				perform pangya.ProcInsertCommand(4, _IDUSER, _MSGID, 0, 0, 0, 1, 0, 1, null);

			END IF; -- IF MSG IS IS OK
		
		-- IF CHECK ITENS IS OK TO SEND
		ELSE
			
			-- Não tem itens para enviar só invia uma mensagem
			INSERT INTO pangya.pangya_gift_table(UID, fromid, message, giftdate, Lida_YN, valid, Flag, Contador_Vista)
			VALUES(_IDUSER, '@SUPERSS', _MSG, now(), 0, 1, 0, 0)
			RETURNING Msg_ID INTO _MSGID;

			IF _MSGID > 0 THEN
				-- Log
				INSERT INTO pangya.pangya_gm_gift_web_log(GM_UID, PLAYER_UID, MSG_ID) VALUES(_ADM_UID, _IDUSER, _MSGID);

				-- Comando para mostrar no jogo que o player recebeu um presente
				perform pangya.ProcInsertCommand(4, _IDUSER, _MSGID, 0, 0, 0, 1, 0, 1, null);
			END IF;

		END IF;	-- ELSE CHECK ITENS IS OK TO SEND

	END; -- IFF CHECK UID
	END IF;
END;
$$;


ALTER FUNCTION pangya.procregistergmgiftweb(_adm_uid integer, _iduser integer, _msg text, _item_typeid_1 integer, _item_typeid_2 integer, _item_typeid_3 integer, _item_typeid_4 integer, _item_typeid_5 integer, _item_qntd_1 integer, _item_qntd_2 integer, _item_qntd_3 integer, _item_qntd_4 integer, _item_qntd_5 integer) OWNER TO postgres;

--
-- TOC entry 763 (class 1255 OID 23449)
-- Name: procregistergrandzodiaceventwebplayerwin(integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procregistergrandzodiaceventwebplayerwin(_uid integer, _item_typeid integer, _item_qntd integer, _item_type integer) RETURNS TABLE("_MSG_ID_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _UID > 0 AND _ITEM_TYPEID > 0 AND _ITEM_QNTD > 0 THEN
	DECLARE
		_MSGID INT = 0;
	BEGIN

		-- Make Mail in gift table
		INSERT INTO pangya.pangya_gift_table(UID, fromid, message, giftdate, Lida_YN, valid, Flag, Contador_Vista)
		VALUES(_UID, '@SUPERSS', 'Grand Zodiac HIO Event', now(), 0, 0/*Não válido, depois eu libero ele se estiver tudo certo*/, 0, 0)
		RETURNING Msg_ID INTO _MSGID;

		IF _MSGID > 0 THEN

			-- ADD Item
			perform pangya.ProcInsertItemNoEmail(0, _UID, _MSGID, -1, _ITEM_TYPEID, 0, _ITEM_QNTD, 0);

			-- Verifica se adicionou
			IF (SELECT COUNT(Msg_ID) FROM pangya.pangya_item_mail WHERE Msg_ID = _MSGID) = 1 THEN
					
				IF _ITEM_TYPE = 1 THEN
					INSERT INTO pangya.pangya_gz_event_2016121600_rare_win(UID, ITEM_TYPEID) VALUES(_UID, _ITEM_TYPEID);
				END IF;

				UPDATE pangya.pangya_gift_table SET valid = 1 WHERE Msg_ID = _MSGID;

				-- Comando para mostrar no jogo que o player recebeu um presente
				perform pangya.ProcInsertCommand(4, _UID, _MSGID, 0, 0, 0, 1, 0, 1, null);

				-- Retorna a MSG ID e o LOG ID
				RETURN QUERY SELECT _MSGID AS MSG_ID;
			ELSE
				-- Error ao salvar os itens, excluí ele e o mail
				DELETE FROM pangya.pangya_item_mail WHERE Msg_ID = _MSGID;

				DELETE FROM pangya.pangya_gift_table WHERE Msg_ID = _MSGID;
			END IF;
		END IF;
	END;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procregistergrandzodiaceventwebplayerwin(_uid integer, _item_typeid integer, _item_qntd integer, _item_type integer) OWNER TO postgres;

--
-- TOC entry 764 (class 1255 OID 23450)
-- Name: procregisterguildmatch(integer, integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procregisterguildmatch(_guild_1_uid integer, _guild_1_point integer, _guild_1_pang integer, _guild_2_uid integer, _guild_2_point integer, _guild_2_pang integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _GUILD_1_UID > 0 AND _GUILD_2_UID > 0 THEN

		INSERT INTO pangya.pangya_guild_match(guild_1_uid, guild_1_point, guild_1_pang, guild_2_uid, guild_2_point, guild_2_pang)
		VALUES(_GUILD_1_UID, _GUILD_1_POINT, _GUILD_1_PANG, _GUILD_2_UID, _GUILD_2_POINT, _GUILD_2_PANG);

	END IF;
END;
$$;


ALTER FUNCTION pangya.procregisterguildmatch(_guild_1_uid integer, _guild_1_point integer, _guild_1_pang integer, _guild_2_uid integer, _guild_2_point integer, _guild_2_pang integer) OWNER TO postgres;

--
-- TOC entry 765 (class 1255 OID 23451)
-- Name: procregisterkickplayercommand(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procregisterkickplayercommand(_player_uid_in integer, _server_uid_in integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	INSERT INTO pangya.pangya_command(command_id, arg1, target)
         VALUES (6, _player_uid_in, _server_uid_in);
END;
$$;


ALTER FUNCTION pangya.procregisterkickplayercommand(_player_uid_in integer, _server_uid_in integer) OWNER TO postgres;

--
-- TOC entry 758 (class 1255 OID 23452)
-- Name: procregisterlogin(integer, text, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procregisterlogin(_iduser integer, _ip text, _srvid integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	UPDATE pangya.account
		SET 
			LastLogonTime = now(), 
			UserIp = _IP, 
			ServerID = _SrvID, 
			LogonCount = LogonCount + 1
	WHERE UID = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.procregisterlogin(_iduser integer, _ip text, _srvid integer) OWNER TO postgres;

--
-- TOC entry 467 (class 1255 OID 23453)
-- Name: procregisterlogon(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procregisterlogon(_iduser integer, _option integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _OPTION = 0 THEN

        UPDATE pangya.account
            SET 
                Logon = 1
        WHERE UID = _IDUSER;

    ELSE
        UPDATE pangya.account
            SET 
                Logon = 0, 
                LastLeaveTime = now()
        WHERE UID = _IDUSER;
    END IF;
END;
$$;


ALTER FUNCTION pangya.procregisterlogon(_iduser integer, _option integer) OWNER TO postgres;

--
-- TOC entry 468 (class 1255 OID 23454)
-- Name: procregisterlogonserver(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procregisterlogonserver(_iduser integer, _gameserver_id integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDUSER >= 0 THEN
		 UPDATE pangya.account
			SET 
			   game_server_id = _gameserver_id
		 WHERE UID = _IDUSER;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procregisterlogonserver(_iduser integer, _gameserver_id integer) OWNER TO postgres;

--
-- TOC entry 469 (class 1255 OID 23455)
-- Name: procregisterlogonserver(integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procregisterlogonserver(_iduser integer, _gameserver_id text) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDUSER >= 0 THEN
		 UPDATE pangya.account
			SET 
			   game_server_id = _gameserver_id
		 WHERE UID = _IDUSER;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procregisterlogonserver(_iduser integer, _gameserver_id text) OWNER TO postgres;

--
-- TOC entry 766 (class 1255 OID 23456)
-- Name: procregisterluciaattendancereward(integer, integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procregisterluciaattendancereward(_uid integer, _count_item integer, _str text) RETURNS TABLE("_MSG_ID_" bigint, "_LOG_ID_" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _UID > 0 AND _COUNT_ITEM > 0 AND _STR IS NOT NULL AND _STR != '' THEN
	DECLARE
		_MSGID BIGINT = 0;
		_LOGID BIGINT = 0;
		_ITEM text;
		_ITEM_TYPEID INT;
		_ITEM_QNTD INT;
		_ITEM_COUNT INT = 0;
		_ITEM_ID INT = -1;
	BEGIN

		-- Make Mail in gift table
		INSERT INTO pangya.pangya_gift_table(UID, fromid, message, giftdate, Lida_YN, valid, Flag, Contador_Vista)
		VALUES(_UID, '@SUPERSS', 'Lucia Attendance reward', now(), 0, 0/*Não válido, depois eu libero ele se estiver tudo certo*/, 0, 0)
		RETURNING Msg_ID INTO _MSGID;

		IF _MSGID > 0 THEN

			-- Make Log
			INSERT INTO pangya.pangya_lucia_attendance_reward_log(UID, MSG_ID) VALUES(_UID, _MSGID)
			RETURNING index INTO _LOGID;

			IF _LOGID > 0 THEN
			DECLARE
				-- Cursor
				itens CURSOR FOR
					SELECT regexp_split_to_table as Item FROM regexp_split_to_table(_STR, '\|');
			BEGIN

				OPEN itens;
				FETCH NEXT FROM itens INTO _ITEM;
				WHILE found LOOP

					IF _ITEM IS NOT NULL THEN
				
						_ITEM_TYPEID = split_part(_ITEM, ';', 1);
						_ITEM_QNTD = split_part(_ITEM, ';', 2);

						IF _ITEM_TYPEID IS NOT NULL AND _ITEM_QNTD IS NOT NULL THEN
							
							perform pangya.ProcInsertItemNoEmail(0, _UID, _MSGID::int, _ITEM_ID, _ITEM_TYPEID, 0, _ITEM_QNTD, 0);
						
							-- Itens inseridos no mail
							_ITEM_COUNT = _ITEM_COUNT + 1;
						END IF;
					END IF;

					FETCH NEXT FROM itens INTO _ITEM;

				END LOOP;

				-- CLOSE AND DEALLOCATE CURSOR
				CLOSE itens;

				-- Verifica se foi os itens tudo OK
				IF _COUNT_ITEM = _ITEM_COUNT AND _COUNT_ITEM = (SELECT COUNT(Msg_ID) FROM pangya.pangya_item_mail WHERE Msg_ID = _MSGID) THEN			
					-- Tudo ok adiciona os 50CP para o player e libera o mail e envia a msg que um mail foi enserido
					UPDATE pangya.user_info SET Cookie = Cookie + 50 WHERE UID = _UID;

					UPDATE pangya.pangya_gift_table SET valid = 1 WHERE Msg_ID = _MSGID;

					-- Comando para mostrar no jogo que o player recebeu um presente
					perform pangya.ProcInsertCommand(4, _UID, _MSGID::int, 0, 0, 0, 1, 0, 1, null);

					-- Retorna a MSG ID e o LOG ID
					RETURN QUERY SELECT _MSGID AS MSG_ID, _LOGID AS LOG_ID;

				ELSE
					-- Error ao salvar os itens, excluí ele e o mail e registra no log do lucia attendance que deu erro
					DELETE FROM pangya.pangya_item_mail WHERE Msg_ID = _MSGID;

					DELETE FROM pangya.pangya_gift_table WHERE Msg_ID = _MSGID;

					UPDATE pangya.pangya_lucia_attendance_reward_log SET ERROR = 'FAIL TO INSERT ITENS' WHERE index = _LOGID;

					--RAISE NOTICE '-UM-';
				END IF;

			END;
			ELSE
				-- Erro ao criar o log, deleta o mail criado
				DELETE FROM pangya.pangya_gift_table WHERE Msg_ID = _MSGID;

				--RAISE NOTICE '-DOIS-';
			END IF;
		END IF;
	END;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procregisterluciaattendancereward(_uid integer, _count_item integer, _str text) OWNER TO postgres;

--
-- TOC entry 767 (class 1255 OID 23457)
-- Name: procregisternewrateserver(integer, integer, integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procregisternewrateserver(_tipo_rate integer, _quantidade integer, _target_in integer, _reservedate_in text) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	INSERT INTO pangya.pangya_command(
         command_id, 
         arg1, 
         arg2, 
         target, 
         reserveDate)
         VALUES (
            5, 
            _tipo_rate, 
            _quantidade, 
            _target_in, 
            _reserveDate_in::timestamp);
END;
$$;


ALTER FUNCTION pangya.procregisternewrateserver(_tipo_rate integer, _quantidade integer, _target_in integer, _reservedate_in text) OWNER TO postgres;

--
-- TOC entry 768 (class 1255 OID 23458)
-- Name: procregisternoticebroadcast(text, integer, integer, integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procregisternoticebroadcast(_msg_in text, _replay_count_in integer, _refresh_time_min_in integer, _target_in integer, _reservedate_in text) RETURNS void
    LANGUAGE plpgsql
    AS $$
DECLARE
	_NOTICE_ID BIGINT = -1;
BEGIN
	INSERT INTO pangya.pangya_notice_list(message, replayCount, refreshTime)
         VALUES (_msg_in, _replay_count_in, _refresh_time_min_in)
		 RETURNING notice_id INTO _NOTICE_ID; 

      INSERT INTO pangya.pangya_command(command_id, arg1, target, reserveDate)
         VALUES (0, _NOTICE_ID, _target_in, _reserveDate_in::timestamp);  
END;
$$;


ALTER FUNCTION pangya.procregisternoticebroadcast(_msg_in text, _replay_count_in integer, _refresh_time_min_in integer, _target_in integer, _reservedate_in text) OWNER TO postgres;

--
-- TOC entry 769 (class 1255 OID 23459)
-- Name: procregisterreloadsystemserver(integer, integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procregisterreloadsystemserver(_sistema integer, _target_in integer, _reservedate_in text) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	INSERT INTO pangya.pangya_command(command_id, arg1, target, reserveDate)
		VALUES(7, _SISTEMA, _TARGET_IN, _RESERVEDATE_IN::timestamp);
END;
$$;


ALTER FUNCTION pangya.procregisterreloadsystemserver(_sistema integer, _target_in integer, _reservedate_in text) OWNER TO postgres;

--
-- TOC entry 770 (class 1255 OID 23460)
-- Name: procregistershutdownserver(text, integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procregistershutdownserver(_date_shutdown_in text, _target_in integer, _reservedate_in text) RETURNS void
    LANGUAGE plpgsql
    AS $$
DECLARE
	_SHUTDOWN_ID BIGINT;
BEGIN
	INSERT INTO pangya.pangya_shutdown_list(date_shutdown)
         VALUES (_date_shutdown_in::timestamp)
		 RETURNING id INTO _SHUTDOWN_ID;
		 
      INSERT INTO pangya.pangya_command(command_id, arg1, target, reserveDate)
         VALUES (3, _SHUTDOWN_ID, _target_in, _reserveDate_in::timestamp);
END;
$$;


ALTER FUNCTION pangya.procregistershutdownserver(_date_shutdown_in text, _target_in integer, _reservedate_in text) OWNER TO postgres;

--
-- TOC entry 771 (class 1255 OID 23461)
-- Name: procregisterticker(integer, integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procregisterticker(_iduser integer, _server_owner integer, _msg text) RETURNS void
    LANGUAGE plpgsql
    AS $$
DECLARE
	--Register Ticker on pangya_command
	_TICKET_ID BIGINT = 0;
BEGIN
	IF _IDUSER > 0 THEN

		INSERT INTO pangya_ticker_list(MESSAGE, NICK) VALUES(_MSG, (SELECT nick FROM account WHERE UID = _IDUSER))
		RETURNING ticker_id INTO _TICKET_ID;
			
		-- Envia para todos game server exceto o server que enviou para DB que ele mesmo coloca na fila de ticker dele
		INSERT INTO pangya_command(command_id, arg1, arg2, target) VALUES(1, _TICKET_ID, _SERVER_OWNER, 1/*Tipo 1 - Todos Game Server*/);
	END IF;
END;
$$;


ALTER FUNCTION pangya.procregisterticker(_iduser integer, _server_owner integer, _msg text) OWNER TO postgres;

--
-- TOC entry 772 (class 1255 OID 23462)
-- Name: procregisterwinbox(integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procregisterwinbox(_iduser integer, _box_typeid integer, _item_typeid integer, _qntd integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
DECLARE
	_OPT INT = 1;
	_NOTICE_ID BIGINT = -1;
BEGIN
	IF _item_typeid = 436207632 THEN
		_OPT = 2; 
	END IF;

	INSERT INTO pangya.pangya_notice_list(pangya.pangya_notice_list.message)
		SELECT 
		'<PARAMS><BOX_TYPEID>'
			||
		CAST(_box_typeid AS varchar(50))
			||
		'</BOX_TYPEID><NICKNAME>'
			||
		account.NICK
			||
		'</NICKNAME><TYPEID>'
			|| 
		CAST(_item_typeid AS varchar(50))
			||
		'</TYPEID><QTY>'
			||
		CAST(_QNTD AS varchar(50))
			||
		'</QTY></PARAMS>'
		FROM pangya.account
		WHERE account.UID = _IDUSER
	RETURNING notice_id INTO _NOTICE_ID;    

	INSERT INTO pangya.pangya_command(pangya.pangya_command.command_id, pangya.pangya_command.arg1, pangya.pangya_command.arg2, pangya.pangya_command.target)
	VALUES (2, _NOTICE_ID, _OPT, 1);
END;
$$;


ALTER FUNCTION pangya.procregisterwinbox(_iduser integer, _box_typeid integer, _item_typeid integer, _qntd integer) OWNER TO postgres;

--
-- TOC entry 773 (class 1255 OID 23463)
-- Name: procregserver_new(integer, text, text, integer, integer, integer, integer, integer, text, text, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procregserver_new(iduser integer, nome text, ipaddres text, porta integer, tipo integer, maxusuario integer, currusuario integer, pangrat integer, serverversao text, clienteversao text, propriedade integer, angelicwingsnumber integer, eventoflag integer, exprat integer, imgnumber integer, scratchrat integer, masteryrat integer, treasurerat integer, rareitemrat integer, cookieitemrat integer, chuvarat integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
DECLARE
		RegType int;
BEGIN

	SELECT UID INTO RegType
	FROM pangya.pangya_server_list
	WHERE UID = IDUSER;

	IF RegType <> 0 THEN

			UPDATE pangya.pangya_server_list
			   SET 
				  Name = NOME, 
				  IP = ipaddres, 
				  Port = porta, 
				  Type = tipo, 
				  MaxUser = maxusuario, 
				  PangRate = PangRat, 
				  CurrUser = currusuario, 
				  State = 1, 
				  UpdateTime = now(), 
				  ScratchRate = ScratchRat, 
				  ServerVersion = ServerVersao, 
				  ClientVersion = ClienteVersao, 
				  property = propriedade, 
				  AngelicWingsNum = AngelicWingsNumber, 
				  EventFlag = EventoFlag, 
				  ExpRate = ExpRat, 
				  ImgNo = ImgNumber, 
				  MasteryRate = MasteryRat, 
				  TreasureRate = TreasureRat, 
				  ChuvaRate = ChuvaRat, 
				  RareItemRate = RareItemRat, 
				  CookieItemRate = CookieItemRat
			WHERE UID = IDUSER;
	ELSE

			INSERT INTO pangya.pangya_server_list(
			   Name, 
			   UID, 
			   IP, 
			   Port, 
			   MaxUser, 
			   CurrUser, 
			   Type, 
			   UpdateTime, 
			   State, 
			   PangRate, 
			   ServerVersion, 
			   ClientVersion, 
			   property, 
			   AngelicWingsNum, 
			   EventFlag, 
			   ExpRate, 
			   ImgNo, 
			   ScratchRate, 
			   MasteryRate, 
			   TreasureRate, 
			   RareItemRate, 
			   CookieItemRate, 
			   ChuvaRate, 
			   PCBangUser, 
			   ServiceControl, 
			   AppRate, 
			   EventMap, 
			   EventDropRate, 
			   HanbitUser, 
			   ParanUser, 
			   AuthState)
			   VALUES (
				  NOME, 
				  IDUSER, 
				  ipaddres, 
				  porta, 
				  maxusuario, 
				  currusuario, 
				  tipo, 
				  now(), 
				  1, 
				  PangRat, 
				  ServerVersao, 
				  ClienteVersao, 
				  propriedade, 
				  AngelicWingsNumber, 
				  EventoFlag, 
				  ExpRat, 
				  ImgNumber, 
				  ScratchRat, 
				  MasteryRat, 
				  TreasureRat, 
				  RareItemRat, 
				  CookieItemRat, 
				  ChuvaRat, 
				  0, 
				  0, 
				  0, 
				  0, 
				  0, 
				  0, 
				  0, 
				  0);
	END IF;

END;
$$;


ALTER FUNCTION pangya.procregserver_new(iduser integer, nome text, ipaddres text, porta integer, tipo integer, maxusuario integer, currusuario integer, pangrat integer, serverversao text, clienteversao text, propriedade integer, angelicwingsnumber integer, eventoflag integer, exprat integer, imgnumber integer, scratchrat integer, masteryrat integer, treasurerat integer, rareitemrat integer, cookieitemrat integer, chuvarat integer) OWNER TO postgres;

--
-- TOC entry 774 (class 1255 OID 23464)
-- Name: procremoveequipedcard(integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procremoveequipedcard(_iduser integer, _part_id integer, _part_typeid integer, _card_typeid integer, _card_slot integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDUSER > 0 AND _PART_ID > 0 AND _PART_TYPEID > 0 AND _CARD_TYPEID > 0 THEN
		UPDATE pangya.pangya_card_equip SET USE_YN = 0
		WHERE UID = _IDUSER AND parts_id = _PART_ID AND parts_typeid = _PART_TYPEID
			AND card_typeid = _CARD_TYPEID AND Slot = _CARD_SLOT;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procremoveequipedcard(_iduser integer, _part_id integer, _part_typeid integer, _card_typeid integer, _card_slot integer) OWNER TO postgres;

--
-- TOC entry 470 (class 1255 OID 23465)
-- Name: procremoveoldandaddnewdailyquestplayer(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procremoveoldandaddnewdailyquestplayer(_iduser integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	DELETE 
	FROM pangya.achievement_quest
	WHERE achievement_quest.UID = _IDUSER AND achievement_quest.ID_ACHIEVEMENT IN 
	(
		SELECT achievement_tipo.ID_ACHIEVEMENT
		FROM pangya.achievement_tipo
		WHERE achievement_tipo.UID = _IDUSER AND achievement_tipo.Option = 1
	);

	DELETE 
	FROM pangya.achievement_tipo
	WHERE achievement_tipo.UID = _IDUSER AND achievement_tipo.Option = 1;

	INSERT INTO pangya.achievement_tipo(
		pangya.achievement_tipo.UID, 
		pangya.achievement_tipo.Nome, 
		pangya.achievement_tipo.TypeID, 
		pangya.achievement_tipo.TIPO, 
		pangya.achievement_tipo.Option)
	SELECT 
		_IDUSER, 
		b.nome, 
		a.achieve_quest_1, 
		1, 
		1
	FROM pangya.pangya_daily_quest  AS a, pangya.quest_items  AS b
	WHERE a.achieve_quest_1 = b.typeid;

	INSERT INTO pangya.achievement_tipo(
		pangya.achievement_tipo.UID, 
		pangya.achievement_tipo.Nome, 
		pangya.achievement_tipo.TypeID, 
		pangya.achievement_tipo.TIPO, 
		pangya.achievement_tipo.Option)
	SELECT 
		_IDUSER, 
		b.nome, 
		a.achieve_quest_2, 
		1, 
		1
	FROM pangya.pangya_daily_quest  AS a, pangya.quest_items  AS b
	WHERE a.achieve_quest_2 = b.typeid;

	INSERT INTO pangya.achievement_tipo(
		pangya.achievement_tipo.UID, 
		pangya.achievement_tipo.Nome, 
		pangya.achievement_tipo.TypeID, 
		pangya.achievement_tipo.TIPO, 
		pangya.achievement_tipo.Option)
	SELECT 
		_IDUSER, 
		b.nome, 
		a.achieve_quest_3, 
		1, 
		1
	FROM pangya.pangya_daily_quest  AS a, pangya.quest_items  AS b
	WHERE a.achieve_quest_3 = b.typeid;
		 
	IF 
	(
		SELECT pangya_daily_quest_player.uid
		FROM pangya.pangya_daily_quest_player
		WHERE pangya_daily_quest_player.uid = _IDUSER
	) IS NULL THEN
		INSERT INTO pangya.pangya_daily_quest_player(pangya.pangya_daily_quest_player.uid, pangya.pangya_daily_quest_player.last_quest_accept, pangya.pangya_daily_quest_player.today_quest)
		VALUES (_IDUSER, NULL, now());
	ELSE 
		UPDATE pangya.pangya_daily_quest_player
			SET 
				today_quest = now()
		WHERE pangya_daily_quest_player.uid = _IDUSER;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procremoveoldandaddnewdailyquestplayer(_iduser integer) OWNER TO postgres;

--
-- TOC entry 775 (class 1255 OID 23466)
-- Name: procrequestjoinguild(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procrequestjoinguild(_player_uid integer, _guild_uid integer) RETURNS TABLE("_RET_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_RET INT = -1;
BEGIN
	IF _PLAYER_UID > 0 AND _GUILD_UID > 0 THEN
	DECLARE
		_OTHER_GUILD INT = (
			SELECT
				a.Guild_UID
			FROM
				pangya.account a
				INNER JOIN
				pangya.pangya_guild b
				ON a.Guild_UID = b.GUILD_UID
			WHERE a.UID = _PLAYER_UID AND 
				(b.GUILD_STATE NOT IN(4, 5) OR b.GUILD_CLOSURE_DATE IS NULL OR now() < b.GUILD_CLOSURE_DATE)
		);
	BEGIN

		IF _OTHER_GUILD IS NULL OR _OTHER_GUILD = _GUILD_UID THEN

			IF (SELECT GUILD_UID FROM pangya.pangya_guild WHERE GUILD_UID = _GUILD_UID AND GUILD_PERMITION_JOIN = 1 AND GUILD_STATE IN(0, 1) AND GUILD_CLOSURE_DATE IS NULL) IS NOT NULL THEN

				IF (SELECT MEMBER_UID FROM pangya.pangya_guild_member WHERE GUILD_UID = _GUILD_UID AND MEMBER_UID = _PLAYER_UID) IS NULL THEN

					-- Delete ele de qualquer membro de guild que ele esteja, que o OTHER_GUILD é null, se ele estava em uma guild, ela já está fechada
					DELETE FROM pangya.pangya_guild_member WHERE MEMBER_UID = _PLAYER_UID;

					-- Adiciona ele para a nova Guild
					INSERT INTO pangya.pangya_guild_member(GUILD_UID, MEMBER_UID, MEMBER_MSG, MEMBER_STATE_FLAG, GUILD_PANG, GUILD_POINT)
					VALUES (_GUILD_UID, _PLAYER_UID, '', 9, 0, 0);

					INSERT INTO pangya.pangya_guild_atividade_player(UID, GUILD_UID, FLAG)
					VALUES (_PLAYER_UID, _GUILD_UID, 1);

					UPDATE pangya.account
						SET 
							GUILD_UID = _GUILD_UID
					WHERE account.UID = _PLAYER_UID;

					_RET = 1;
				ELSE
					_RET = -2;	-- Já fez o pedido para entrar nessa guild
				END IF;

			ELSE
				_RET = -3;	-- A guild que ele quer entrar está fechada ou bloqueada
			END IF;

		ELSE
			_RET = -4;	-- Player já está em outra guild
		END IF;

	END;
	END IF;

	RETURN QUERY SELECT _RET AS RET;
END;
$$;


ALTER FUNCTION pangya.procrequestjoinguild(_player_uid integer, _guild_uid integer) OWNER TO postgres;

--
-- TOC entry 776 (class 1255 OID 23467)
-- Name: procrescueid(text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procrescueid(_email text) RETURNS TABLE("_RET_" integer, "_UID_" bigint, "_INDEX_" bigint, "_KEY_" character varying, "_EMAIL_" text)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_RET INT = -1;
	_KEY VARCHAR(100) = '';
	_INDEX BIGINT = 0;
	_UID BIGINT = 0;
BEGIN
	IF _EMAIL != '' THEN

		_UID = (SELECT UID FROM pangya.contas_beta WHERE email = _EMAIL);

		IF _UID IS NOT NULL AND _UID > 0 THEN
			
			IF (SELECT UID FROM pangya.pangya_rescue_pwd_log WHERE UID = _UID AND tipo = 2 AND state = 0 AND (send_date + '1 hour'::interval) >= now() LIMIT 1) IS NULL THEN

				INSERT INTO pangya.pangya_rescue_pwd_log(UID, tipo) VALUES(_UID, 2/*ID*/)
				RETURNING index INTO _INDEX;

				_KEY = CAST((SELECT key_uniq FROM pangya.pangya_rescue_pwd_log WHERE index = _INDEX AND UID = _UID) AS VARCHAR(100));
				
				-- Sucesso ao criar o log e a chave
				_RET = 1;
			ELSE
				_RET = -4;	-- Já enviou um email com as instruções para recuperar a senha
			END IF;
		ELSE
			_RET = -3;	-- Email invalido
		END IF;

	END IF;

	RETURN QUERY SELECT _RET AS RET, _UID AS UID, _INDEX AS INDEX, _KEY AS KEY, _EMAIL as EMAIL;
END;
$$;


ALTER FUNCTION pangya.procrescueid(_email text) OWNER TO postgres;

--
-- TOC entry 777 (class 1255 OID 23468)
-- Name: procrescuepassword(text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procrescuepassword(_id text) RETURNS TABLE("_RET_" integer, "_UID_" bigint, "_INDEX_" bigint, "_KEY_" character varying, "_EMAIL_" character varying)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_RET INT = -1;
	_KEY VARCHAR(100) = '';
	_INDEX BIGINT = 0;
	_EMAIL VARCHAR(100) = NULL;
	_UID BIGINT = 0;
BEGIN
	IF _ID != '' THEN

		_UID = (SELECT UID FROM pangya.account WHERE ID = _ID);

		IF _ID IS NOT NULL THEN

			_EMAIL = (SELECT email FROM pangya.contas_beta WHERE UID = _UID);

			IF _EMAIL IS NOT NULL THEN

				IF (SELECT UID FROM pangya.pangya_rescue_pwd_log WHERE UID = _UID AND tipo = 1 AND state = 0 AND (send_date + '1 hour'::interval) >= now() LIMIT 1) IS NULL THEN

					INSERT INTO pangya.pangya_rescue_pwd_log(UID, tipo) VALUES(_UID, 1/*Password*/)
					RETURNING index INTO _INDEX;

					_KEY = CAST((SELECT key_uniq FROM pangya.pangya_rescue_pwd_log WHERE index = _INDEX AND UID = _UID) AS VARCHAR(100));

					-- Sucesso ao criar o log e a chave
					_RET = 1;
				ELSE
					_RET = -4;	-- Já enviou um email com as instruções para recuperar a senha
				END IF;

			ELSE
				_RET = -3;	-- Email invalido
			END IF;
		ELSE
			_RET = -2;	-- ID invalido
		END IF;

	END IF;

	RETURN QUERY SELECT _RET AS RET, _UID AS UID, _INDEX AS INDEX, _KEY AS KEY, _EMAIL AS EMAIL;
END;
$$;


ALTER FUNCTION pangya.procrescuepassword(_id text) OWNER TO postgres;

--
-- TOC entry 778 (class 1255 OID 23469)
-- Name: procsaiguild(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procsaiguild(_iduser integer, _uid_guild integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	UPDATE pangya.account
		SET 
		GUILD_UID = 0
	WHERE UID = _IDUSER;

	DELETE 
	FROM pangya.pangya_guild_member
	WHERE GUILD_UID = _UID_GUILD AND MEMBER_UID = _IDUSER;

	INSERT INTO pangya.pangya_guild_atividade_player(UID, GUILD_UID, FLAG)
		VALUES (_IDUSER, _UID_GUILD, 7);

	-- Cria log de update para o Game Server verificar e pedir para o Message Server Atualize os Membros do Club
	INSERT INTO pangya.pangya_guild_update_activity(GUILD_UID, OWNER_UPDATE, PLAYER_UID, TYPE_UPDATE) VALUES(_UID_GUILD, _IDUSER/*O próprio player que saiu da Guild*/, _IDUSER, 1/*Sai Club*/);
END;
$$;


ALTER FUNCTION pangya.procsaiguild(_iduser integer, _uid_guild integer) OWNER TO postgres;

--
-- TOC entry 779 (class 1255 OID 23470)
-- Name: procsalvadadosplayerguild(integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procsalvadadosplayerguild(_iduser integer, _uid_guild integer, _point_guild integer, _pang_guild integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	UPDATE pangya.pangya_guild
         SET 
            GUILD_POINT = pangya_guild.GUILD_POINT + _POINT_GUILD, 
            GUILD_PANG = pangya_guild.GUILD_PANG + _PANG_GUILD
      WHERE pangya_guild.GUILD_UID = _UID_GUILD;

      UPDATE pangya.pangya_guild_member
         SET 
            GUILD_POINT = pangya_guild_member.GUILD_POINT + _POINT_GUILD, 
            GUILD_PANG = pangya_guild_member.GUILD_PANG + _PANG_GUILD
      WHERE pangya_guild_member.GUILD_UID = _UID_GUILD AND pangya_guild_member.MEMBER_UID = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.procsalvadadosplayerguild(_iduser integer, _uid_guild integer, _point_guild integer, _pang_guild integer) OWNER TO postgres;

--
-- TOC entry 780 (class 1255 OID 23471)
-- Name: procsavenickname(integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procsavenickname(_uid integer, _nickname text) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _UID > 0 AND _NICKNAME != N'' THEN
		UPDATE pangya.account SET nick = _NICKNAME WHERE UID = _UID;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procsavenickname(_uid integer, _nickname text) OWNER TO postgres;

--
-- TOC entry 783 (class 1255 OID 23472)
-- Name: procsendgachajpplayeritemtomail(integer, bigint); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procsendgachajpplayeritemtomail(_uid integer, _id bigint) RETURNS TABLE("_MSG_ID_" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _UID > 0 AND _ID > 0 THEN

		IF (SELECT index FROM pangya.pangya_gacha_jp_player_win WHERE UID = _UID AND index = _ID AND valid = 1 AND send_mail = 0 LIMIT 1) IS NOT NULL THEN
		DECLARE
			_MSGID BIGINT = 0;
		BEGIN

			-- Make Mail in gift table
			INSERT INTO pangya.pangya_gift_table(UID, fromid, message, giftdate, Lida_YN, valid, Flag, Contador_Vista)
			VALUES(_UID, '@SUPERSS', 'Gacha Item', now(), 0, 0/*Não válido, depois eu libero ele se estiver tudo certo*/, 0, 0)
			RETURNING Msg_ID INTO _MSGID;

			IF _MSGID > 0 THEN
			DECLARE
				_ITEM_ID INT = -1;
				_ITEM_TYPEID INT = 0;
				_ITEM_QNTD INT = 0;
			BEGIN
				
				SELECT typeid, qnty INTO _ITEM_TYPEID, _ITEM_QNTD
				FROM pangya.pangya_gacha_jp_player_win 
				WHERE UID = _UID AND index = _ID AND valid = 1 AND send_mail = 0 LIMIT 1;

				-- ADD Item
				perform pangya.ProcInsertItemNoEmail(0, _UID, _MSGID::int, _ITEM_ID, _ITEM_TYPEID, 0, _ITEM_QNTD, 0);

				-- Verifica se adicionou
				IF (SELECT COUNT(Msg_ID) FROM pangya.pangya_item_mail WHERE Msg_ID = _MSGID) = 1 THEN
					
					UPDATE pangya.pangya_gacha_jp_player_win SET send_mail = 1 WHERE UID = _UID AND index = _ID;

					UPDATE pangya.pangya_gift_table SET valid = 1 WHERE Msg_ID = _MSGID;

					-- Comando para mostrar no jogo que o player recebeu um presente
					perform pangya.ProcInsertCommand(4, _UID, _MSGID::int, 0, 0, 0, 1, 0, 1, null);

					-- Retorna a MSG ID e o LOG ID
					RETURN QUERY SELECT _MSGID AS MSG_ID;
				ELSE
					-- Error ao salvar os itens, excluí ele e o mail
					DELETE FROM pangya.pangya_item_mail WHERE Msg_ID = _MSGID;

					DELETE FROM pangya.pangya_gift_table WHERE Msg_ID = _MSGID;
				END IF;
			END;
			END IF;
		END;
		END IF;

	END IF;
END;
$$;


ALTER FUNCTION pangya.procsendgachajpplayeritemtomail(_uid integer, _id bigint) OWNER TO postgres;

--
-- TOC entry 784 (class 1255 OID 23473)
-- Name: procsetassist(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procsetassist(_iduser integer) RETURNS TABLE("_ASSISTENTE_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_ASSISTENTE int = 0;
BEGIN
	IF 
	(
		SELECT pangya_assistente.UID
		FROM pangya.pangya_assistente
		WHERE pangya_assistente.UID = _IDUSER
	) IS NULL THEN

		INSERT INTO pangya.pangya_assistente(pangya.pangya_assistente.UID)
		VALUES (_IDUSER);

		_ASSISTENTE = 1;
	ELSIF 
	(
		SELECT pangya_assistente.Assist
		FROM pangya.pangya_assistente
		WHERE pangya_assistente.UID = _IDUSER
	) = 1 THEN

		UPDATE pangya.pangya_assistente
			SET 
				assist = 0
		WHERE pangya_assistente.UID = _IDUSER;

		_ASSISTENTE = 0;
	ELSE

		UPDATE pangya.pangya_assistente
			SET 
				assist = 1
		WHERE pangya_assistente.UID = _IDUSER;

		_ASSISTENTE = 1;

	END IF;

	RETURN QUERY SELECT _ASSISTENTE AS ASSISTENTE;
END;
$$;


ALTER FUNCTION pangya.procsetassist(_iduser integer) OWNER TO postgres;

--
-- TOC entry 785 (class 1255 OID 23474)
-- Name: proctempuplevelclubset(integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.proctempuplevelclubset(_iduser integer, _taqueira_id integer, _estado integer, _typeid_item_usado integer, _qntd integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
DECLARE
	_COUNT bigint = 0;
BEGIN
	SELECT count(*) INTO _COUNT
	FROM pangya.pangya_last_up_clubset
	WHERE pangya_last_up_clubset.UID = _IDUSER;

	IF _COUNT = 0 THEN
		INSERT INTO pangya.pangya_last_up_clubset(
			pangya.pangya_last_up_clubset.item_id, 
			pangya.pangya_last_up_clubset.UID, 
			pangya.pangya_last_up_clubset.State, 
			pangya.pangya_last_up_clubset.item_usado, 
			pangya.pangya_last_up_clubset.Quantidade)
		VALUES (
			_Taqueira_ID, 
			_IDUSER, 
			_Estado, 
			_TYPEID_ITEM_USADO, 
			_QNTD);
	ELSE 
		UPDATE pangya.pangya_last_up_clubset
			SET 
				item_id = _Taqueira_ID, 
				State = _Estado, 
				item_usado = _TYPEID_ITEM_USADO, 
				Quantidade = _QNTD
		WHERE pangya_last_up_clubset.UID = _IDUSER;
	END IF;
END;
$$;


ALTER FUNCTION pangya.proctempuplevelclubset(_iduser integer, _taqueira_id integer, _estado integer, _typeid_item_usado integer, _qntd integer) OWNER TO postgres;

--
-- TOC entry 786 (class 1255 OID 23475)
-- Name: proctransferemasterypts(integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.proctransferemasterypts(_iduser integer, _from_taqueira_id integer, _to_taqueira_id integer, _quantidade integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
DECLARE
	_MASTERY int;
	_MASTERY_TOTAL int;
	_CALC int;
BEGIN
	SELECT pangya_item_warehouse.Mastery_Pts INTO _MASTERY
	FROM pangya.pangya_item_warehouse
	WHERE pangya_item_warehouse.UID = _IDUSER AND pangya_item_warehouse.item_id = _From_Taqueira_ID;

	SELECT pangya_item_warehouse.Total_Mastery_Pts INTO _MASTERY_TOTAL
	FROM pangya.pangya_item_warehouse
	WHERE pangya_item_warehouse.UID = _IDUSER AND pangya_item_warehouse.item_id = _From_Taqueira_ID;

	_CALC = _MASTERY - (300 * _Quantidade);

	_MASTERY_TOTAL = 
		CASE 
			WHEN _CALC < 0 THEN _MASTERY
			ELSE (300 * _Quantidade)
		END;

	IF _CALC < 0 THEN

		UPDATE pangya.pangya_item_warehouse
			SET 
				Mastery_Pts = 0, 
				Total_Mastery_Pts = pangya_item_warehouse.Total_Mastery_Pts - _MASTERY_TOTAL
		WHERE pangya_item_warehouse.UID = _IDUSER AND pangya_item_warehouse.item_id = _From_Taqueira_ID;

		UPDATE pangya.pangya_item_warehouse
			SET 
				Mastery_Pts = pangya_item_warehouse.Mastery_Pts + _MASTERY_TOTAL, 
				Total_Mastery_Pts = pangya_item_warehouse.Total_Mastery_Pts + _MASTERY_TOTAL
		WHERE pangya_item_warehouse.UID = _IDUSER AND pangya_item_warehouse.item_id = _To_Taqueira_ID;

	ELSE

		UPDATE pangya.pangya_item_warehouse
			SET 
				Mastery_Pts = _CALC, 
				Total_Mastery_Pts = pangya_item_warehouse.Total_Mastery_Pts - _MASTERY_TOTAL
		WHERE pangya_item_warehouse.UID = _IDUSER AND pangya_item_warehouse.item_id = _From_Taqueira_ID;

		UPDATE pangya.pangya_item_warehouse
			SET 
				Mastery_Pts = pangya_item_warehouse.Mastery_Pts + _MASTERY_TOTAL, 
				Total_Mastery_Pts = pangya_item_warehouse.Total_Mastery_Pts + _MASTERY_TOTAL
		WHERE pangya_item_warehouse.UID = _IDUSER AND pangya_item_warehouse.item_id = _To_Taqueira_ID;

	END IF;
END;
$$;


ALTER FUNCTION pangya.proctransferemasterypts(_iduser integer, _from_taqueira_id integer, _to_taqueira_id integer, _quantidade integer) OWNER TO postgres;

--
-- TOC entry 787 (class 1255 OID 23476)
-- Name: proctransferpart(integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.proctransferpart(_uid_sell integer, _uid_buy integer, _item_id integer, _item_type_iff integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _UID_SELL > 0 AND _UID_BUY > 0 AND _ITEM_ID > 0 THEN
		UPDATE pangya.pangya_item_warehouse SET UID = _UID_BUY WHERE UID = _UID_SELL AND item_id = _ITEM_ID;

		-- UCC
		IF _ITEM_TYPE_IFF = 8 OR _ITEM_TYPE_IFF = 9 THEN
			UPDATE pangya.tu_ucc SET UID = _UID_BUY WHERE UID = _UID_SELL AND item_id = _ITEM_ID;
		END IF;

	END IF;
END;
$$;


ALTER FUNCTION pangya.proctransferpart(_uid_sell integer, _uid_buy integer, _item_id integer, _item_type_iff integer) OWNER TO postgres;

--
-- TOC entry 788 (class 1255 OID 23477)
-- Name: proctransformeclubsettemp(integer, integer, integer, integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.proctransformeclubsettemp(_iduser integer, _taqueiraid integer, _state_in integer, _mastery_in integer, _state2_in integer, _card_typeid_in integer, _card_qntd_in integer, _taqueira_trans_typeid_in integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	INSERT INTO pangya.pangya_transforme_clubset_temp(
         pangya.pangya_transforme_clubset_temp.UID, 
         pangya.pangya_transforme_clubset_temp.TAQUEIRA_ID, 
         pangya.pangya_transforme_clubset_temp.STATE, 
         pangya.pangya_transforme_clubset_temp.MASTERY, 
         pangya.pangya_transforme_clubset_temp.STATE2, 
         pangya.pangya_transforme_clubset_temp.FLAG, 
         pangya.pangya_transforme_clubset_temp.CARD_TYPEID, 
         pangya.pangya_transforme_clubset_temp.CARD_QNTD, 
         pangya.pangya_transforme_clubset_temp.TAQUEIRA_TRANS_TYPEID)
	 VALUES (
            _IDUSER, 
            _taqueiraid, 
            _state_in, 
            _mastery_in, 
            _state2_in, 
            0, 
            _card_typeid_in, 
            _card_qntd_in, 
            _taqueira_trans_typeid_in);
END;
$$;


ALTER FUNCTION pangya.proctransformeclubsettemp(_iduser integer, _taqueiraid integer, _state_in integer, _mastery_in integer, _state2_in integer, _card_typeid_in integer, _card_qntd_in integer, _taqueira_trans_typeid_in integer) OWNER TO postgres;

--
-- TOC entry 472 (class 1255 OID 23478)
-- Name: proctrocanickname(integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.proctrocanickname(_iduser integer, _nickname text) RETURNS TABLE("_RET_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_RET int = 0;
BEGIN
	IF _nickname IS NOT NULL THEN

		UPDATE pangya.account
			SET 
				nick = _nickname, 
				CHANGE_NICK = now()
		WHERE account.UID = _IDUSER;

		_RET = 1;

	END IF;

	RETURN QUERY SELECT _RET AS RET;
END;
$$;


ALTER FUNCTION pangya.proctrocanickname(_iduser integer, _nickname text) OWNER TO postgres;

--
-- TOC entry 604 (class 1255 OID 23479)
-- Name: procupdateattendancereward(integer, integer, integer, integer, integer, integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdateattendancereward(_iduser integer, _counter integer, _item_typeid_now integer, _item_qntd_now integer, _item_typeid_after integer, _item_qntd_after integer, _last_login text) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDUSER > 0 THEN
		UPDATE pangya.pangya_attendance_reward
			SET counter = _COUNTER,
				item_typeid_now = _ITEM_TYPEID_NOW,
				item_qntd_now = _ITEM_QNTD_NOW,
				item_typeid_after = _ITEM_TYPEID_AFTER,
				item_qntd_after = _ITEM_QNTD_AFTER,
				last_login = _LAST_LOGIN::timestamp
		WHERE UID = _IDUSER;
   END IF;
END;
$$;


ALTER FUNCTION pangya.procupdateattendancereward(_iduser integer, _counter integer, _item_typeid_now integer, _item_qntd_now integer, _item_typeid_after integer, _item_qntd_after integer, _last_login text) OWNER TO postgres;

--
-- TOC entry 611 (class 1255 OID 23480)
-- Name: procupdateauthserverkey(integer, text, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdateauthserverkey(_server_uid integer, _key text, _valid integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN

	IF _SERVER_UID > 0 THEN
		UPDATE pangya.pangya_auth_key
			SET key = _KEY,
				valid = _VALID
		WHERE Server_UID = _SERVER_UID;
	END IF;
	
END;
$$;


ALTER FUNCTION pangya.procupdateauthserverkey(_server_uid integer, _key text, _valid integer) OWNER TO postgres;

--
-- TOC entry 649 (class 1255 OID 23481)
-- Name: procupdatebotgmeventreward(bigint, integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdatebotgmeventreward(_id bigint, _typeid integer, _qntd integer, _qntd_time integer, _rate integer, _valid integer) RETURNS TABLE("_ID_" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _ID > 0 AND _TYPEID > 0 THEN

		UPDATE pangya.pangya_bot_gm_event_reward
			SET
				typeid = _TYPEID,
				qntd = _QNTD,
				qntd_time = _QNTD_TIME,
				rate = _RATE,
				valid = _VALID
		WHERE
			index = _ID;

		RETURN QUERY SELECT _ID AS ID;
	ELSE
		RETURN QUERY SELECT -1::bigint AS ID;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procupdatebotgmeventreward(_id bigint, _typeid integer, _qntd integer, _qntd_time integer, _rate integer, _valid integer) OWNER TO postgres;

--
-- TOC entry 781 (class 1255 OID 23482)
-- Name: procupdatebotgmeventtime(bigint, text, text, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdatebotgmeventtime(_id bigint, _inicio_time text, _fim_time text, _channel_id integer, _valid integer) RETURNS TABLE("_ID_" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _ID > 0 AND _INICIO_TIME IS NOT NULL AND _FIM_TIME IS NOT NULL THEN

		UPDATE pangya.pangya_bot_gm_event_time
			SET
				inicio_time = _INICIO_TIME::time,
				fim_time = _FIM_TIME::time,
				channel_id = _CHANNEL_ID,
				valid = _VALID
		WHERE
			index = _ID;

		RETURN QUERY SELECT _ID AS ID;
	ELSE
		RETURN QUERY SELECT -1::bigint AS ID;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procupdatebotgmeventtime(_id bigint, _inicio_time text, _fim_time text, _channel_id integer, _valid integer) OWNER TO postgres;

--
-- TOC entry 782 (class 1255 OID 23483)
-- Name: procupdatecaddieholyday(integer, integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdatecaddieholyday(_iduser integer, _iditem integer, _end_date text) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	UPDATE pangya.pangya_caddie_information SET EndDate = _END_DATE::timestamp WHERE UID = _IDUSER AND item_id = _IDITEM;
END;
$$;


ALTER FUNCTION pangya.procupdatecaddieholyday(_iduser integer, _iditem integer, _end_date text) OWNER TO postgres;

--
-- TOC entry 789 (class 1255 OID 23484)
-- Name: procupdatecaddieinfo(integer, integer, integer, integer, integer, integer, integer, integer, integer, text, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdatecaddieinfo(_iduser integer, _id integer, _typeid integer, _parts_typeid integer, _level integer, _exp integer, _rent_flag integer, _purchase integer, _check_end integer, _end_dt text, _parts_end_dt text) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDUSER > 0 THEN
		UPDATE pangya.pangya_caddie_information
			SET parts_typeid = _PARTS_TYPEID,
				cLevel = _LEVEL,
				Exp = _EXP,
				RentFlag = _RENT_FLAG,
				Purchase = _PURCHASE,
				CheckEnd = _CHECK_END,
				EndDate = _END_DT::timestamp,
				parts_EndDate = _PARTS_END_DT::timestamp
		WHERE UID = _IDUSER AND item_id = _ID AND typeid = _TYPEID;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procupdatecaddieinfo(_iduser integer, _id integer, _typeid integer, _parts_typeid integer, _level integer, _exp integer, _rent_flag integer, _purchase integer, _check_end integer, _end_dt text, _parts_end_dt text) OWNER TO postgres;

--
-- TOC entry 790 (class 1255 OID 23485)
-- Name: procupdatecaddieitem(integer, integer, integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdatecaddieitem(_iduser integer, _iditem integer, _typeid integer, _end_date text) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	UPDATE pangya.pangya_caddie_information SET parts_typeid = _TYPEID, parts_enddate = _END_DATE::timestamp WHERE UID = _IDUSER AND item_id = _IDITEM;
END;
$$;


ALTER FUNCTION pangya.procupdatecaddieitem(_iduser integer, _iditem integer, _typeid integer, _end_date text) OWNER TO postgres;

--
-- TOC entry 791 (class 1255 OID 23486)
-- Name: procupdatecardspecialtime(integer, bigint, integer, integer, integer, integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdatecardspecialtime(_iduser integer, _index bigint, _item_typeid integer, _efeito_type integer, _efeito_qntd integer, _tipo integer, _end_date text) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDUSER > 0 AND _INDEX > 0 THEN
		UPDATE pangya.pangya_card_equip 
			SET card_typeid = _ITEM_TYPEID,
				Efeito = _EFEITO_TYPE,
				Efeito_Qntd = _EFEITO_QNTD,
				Tipo = _TIPO,
				END_DT = COALESCE(_END_DATE::timestamp, now())
		WHERE UID = _IDUSER AND index = _INDEX  AND use_yn = 1;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procupdatecardspecialtime(_iduser integer, _index bigint, _item_typeid integer, _efeito_type integer, _efeito_qntd integer, _tipo integer, _end_date text) OWNER TO postgres;

--
-- TOC entry 792 (class 1255 OID 23487)
-- Name: procupdatecharacterpcl(integer, integer, integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdatecharacterpcl(_iduser integer, _item_id integer, _c0 integer, _c1 integer, _c2 integer, _c3 integer, _c4 integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDUSER > 0 AND _ITEM_ID > 0 THEN
		UPDATE pangya.pangya_character_information SET PCL0 = _C0, PCL1 = _C1, PCL2 = _C2, PCL3 = _C3, PCL4 = _C4
		WHERE UID = _IDUSER AND item_id = _ITEM_ID;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procupdatecharacterpcl(_iduser integer, _item_id integer, _c0 integer, _c1 integer, _c2 integer, _c3 integer, _c4 integer) OWNER TO postgres;

--
-- TOC entry 793 (class 1255 OID 23488)
-- Name: procupdatechatmacrouser(integer, text, text, text, text, text, text, text, text, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdatechatmacrouser(_iduser integer, _m1 text, _m2 text, _m3 text, _m4 text, _m5 text, _m6 text, _m7 text, _m8 text, _m9 text) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDUSER > 0 THEN
		UPDATE pangya.pangya_user_macro 
			SET  Macro1 = _M1
				,Macro2 = _M2
				,Macro3 = _M3
				,Macro4 = _M4
				,Macro5 = _M5
				,Macro6 = _M6
				,Macro7 = _M7
				,Macro8 = _M8
				,Macro9 = _M9
		WHERE UID = _IDUSER;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procupdatechatmacrouser(_iduser integer, _m1 text, _m2 text, _m3 text, _m4 text, _m5 text, _m6 text, _m7 text, _m8 text, _m9 text) OWNER TO postgres;

--
-- TOC entry 794 (class 1255 OID 23489)
-- Name: procupdateclubsetstats(integer, integer, bigint, integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdateclubsetstats(_iduser integer, _item_id integer, _pang bigint, _c0 integer, _c1 integer, _c2 integer, _c3 integer, _c4 integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDUSER > 0 AND _ITEM_ID > 0 THEN
		UPDATE pangya.pangya_item_warehouse SET C0 = _C0, C1 = _C1, C2 = _C2, C3 = _C3, C4 = _C4
		WHERE UID = _IDUSER AND item_id = _ITEM_ID;

		IF _PANG > 0 THEN
			IF (SELECT UID FROM pangya.pangya_clubset_enchant WHERE UID = _IDUSER AND item_id = _ITEM_ID LIMIT 1) IS NOT NULL THEN
				INSERT INTO pangya.pangya_clubset_enchant(UID, item_id) VALUES(_IDUSER, _ITEM_ID);
			END IF;

			UPDATE pangya.pangya_clubset_enchant SET pang = pang + pang WHERE UID = _IDUSER AND item_id = _ITEM_ID;
		END IF;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procupdateclubsetstats(_iduser integer, _item_id integer, _pang bigint, _c0 integer, _c1 integer, _c2 integer, _c3 integer, _c4 integer) OWNER TO postgres;

--
-- TOC entry 795 (class 1255 OID 23490)
-- Name: procupdateclubsettime(integer, integer, integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdateclubsettime(_iduser integer, _iditem integer, _itemtypeid integer, _end_date text) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	UPDATE pangya.pangya_item_warehouse SET EndDate = _END_DATE::timestamp WHERE UID = _IDUSER AND item_id = _IDITEM AND typeid = _ITEMTYPEID;
END;
$$;


ALTER FUNCTION pangya.procupdateclubsettime(_iduser integer, _iditem integer, _itemtypeid integer, _end_date text) OWNER TO postgres;

--
-- TOC entry 796 (class 1255 OID 23491)
-- Name: procupdateclubsetworkshop(integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdateclubsetworkshop(_iduser integer, _item_id integer, _level integer, _c0 integer, _c1 integer, _c2 integer, _c3 integer, _c4 integer, _mastery integer, _rank integer, _recovery integer, _flag integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDUSER > 0 AND _ITEM_ID > 0 THEN
		
		IF _FLAG = 4 THEN	-- UP RANK CLUBSET tira Mastery e add aos mastery GASTO
			UPDATE pangya.pangya_item_warehouse
			SET Mastery_Gasto = Mastery_Gasto + (Mastery_Pts - _MASTERY)	-- Aqui tem que add, diminui o Mastery, mas tem que aumentar no Mastery Gasto
			WHERE UID = _IDUSER AND item_id = _ITEM_ID;
		ELSIF _FLAG = 0 THEN	-- TRANSFER MASTERY POINTS
			UPDATE pangya.pangya_item_warehouse
			SET Total_Mastery_Pts = Total_Mastery_Pts + (_MASTERY - Mastery_Pts) -- ATT o TOTAL de Mastery Pts
			WHERE UID = _IDUSER AND item_id = _ITEM_ID;
		END IF;

		UPDATE pangya.pangya_item_warehouse 
		SET ClubSet_WorkShop_C0 = _C0,
			ClubSet_WorkShop_C1 = _C1,
			ClubSet_WorkShop_C2 = _C2,
			ClubSet_WorkShop_C3 = _C3,
			ClubSet_WorkShop_C4 = _C4,
			Level = _LEVEL,
			Mastery_Pts = _MASTERY,
			Up = _RANK,	-- UP é o RANK
			Recovery_Pts = _RECOVERY
		WHERE UID = _IDUSER AND item_id = _ITEM_ID;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procupdateclubsetworkshop(_iduser integer, _item_id integer, _level integer, _c0 integer, _c1 integer, _c2 integer, _c3 integer, _c4 integer, _mastery integer, _rank integer, _recovery integer, _flag integer) OWNER TO postgres;

--
-- TOC entry 797 (class 1255 OID 23492)
-- Name: procupdatecoincubelocation(bigint, integer, integer, integer, integer, bigint, double precision, double precision, double precision); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdatecoincubelocation(__id bigint, __course integer, __hole integer, __tipo integer, __tipo_location integer, __rate bigint, __x double precision, __y double precision, __z double precision) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF __ID > 0 THEN

		-- Update
		UPDATE
			pangya.pangya_coin_cube_location
		SET
			course = __COURSE,
			hole = __HOLE,
			tipo = __TIPO,
			tipo_location = __TIPO_LOCATION,
			rate = __RATE,
			x = __X,
			y = __Y,
			z = __z,
			reg_date = now()
		WHERE
			index = __ID;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procupdatecoincubelocation(__id bigint, __course integer, __hole integer, __tipo integer, __tipo_location integer, __rate bigint, __x double precision, __y double precision, __z double precision) OWNER TO postgres;

--
-- TOC entry 798 (class 1255 OID 23493)
-- Name: procupdatecommand(integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdatecommand(_idx integer, _command_id integer, _arg1 integer, _arg2 integer, _arg3 integer, _arg4 integer, _arg5 integer, _target integer, _flag integer, _valid integer, _reservedate text) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDX > 0 THEN

		UPDATE pangya.pangya_command
			SET command_id = _COMMAND_ID,
				arg1 = _ARG1,
				arg2 = _ARG2,
				arg3 = _ARG3,
				arg4 = _ARG4,
				arg5 = _ARG5,
				target = _TARGET,
				flag = _FLAG,
				valid = _VALID,
				reserveDate = _RESERVEDATE::timestamp
		WHERE idx = _IDX;

	END IF;
END;
$$;


ALTER FUNCTION pangya.procupdatecommand(_idx integer, _command_id integer, _arg1 integer, _arg2 integer, _arg3 integer, _arg4 integer, _arg5 integer, _target integer, _flag integer, _valid integer, _reservedate text) OWNER TO postgres;

--
-- TOC entry 799 (class 1255 OID 23494)
-- Name: procupdatecountdayluciaattendance(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdatecountdayluciaattendance(_uid integer, _count_day integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _UID > 0 THEN

		UPDATE pangya.pangya_lucia_attendance
			SET count_day = _COUNT_DAY,
				last_day_attendance = now()
		WHERE UID = _UID;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procupdatecountdayluciaattendance(_uid integer, _count_day integer) OWNER TO postgres;

--
-- TOC entry 801 (class 1255 OID 23495)
-- Name: procupdatedailyquest(integer, integer, integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdatedailyquest(_achieve_quest_1_in integer, _achieve_quest_2_in integer, _achieve_quest_3_in integer, _reg_dt text) RETURNS TABLE("_STATE_" integer, _achieve_quest_1_ integer, _achieve_quest_2_ integer, _achieve_quest_3_ integer, "_Reg_Date_" timestamp without time zone)
    LANGUAGE plpgsql
    AS $$
BEGIN
	-- Block TABLE
    LOCK TABLE pangya.pangya_daily_quest IN ACCESS EXCLUSIVE MODE;

	IF (SELECT count(*) FROM pangya.pangya_daily_quest) > 0 THEN

		UPDATE pangya.pangya_daily_quest
		SET 
			achieve_quest_1 = _achieve_quest_1_in, 
			achieve_quest_2 = _achieve_quest_2_in, 
			achieve_quest_3 = _achieve_quest_3_in, 
			Reg_Date = _REG_DT::timestamp
		WHERE Reg_Date IS NULL OR (CAST(now() AS DATE) - CAST(Reg_Date AS DATE)) >= 1;

	ELSE
		INSERT INTO pangya.pangya_daily_quest(achieve_quest_1, achieve_quest_2, achieve_quest_3, Reg_Date)
		VALUES (_achieve_quest_1_in, _achieve_quest_2_in, _achieve_quest_3_in, _REG_DT::timestamp);
    END IF;

	IF found THEN
		RETURN QUERY SELECT 1 AS OK, 0::int, 0::int, 0::int, null::timestamp;
	ELSE
		RETURN QUERY SELECT 0 AS OK, * FROM pangya.pangya_daily_quest;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procupdatedailyquest(_achieve_quest_1_in integer, _achieve_quest_2_in integer, _achieve_quest_3_in integer, _reg_dt text) OWNER TO postgres;

--
-- TOC entry 802 (class 1255 OID 23496)
-- Name: procupdatedailyquestuser(bigint, text, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdatedailyquestuser(_uid bigint, _accept text, _today text) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	UPDATE pangya.pangya_daily_quest_player SET last_quest_accept = _ACCEPT::timestamp, today_quest = _TODAY::timestamp WHERE UID = _UID;
END;
$$;


ALTER FUNCTION pangya.procupdatedailyquestuser(_uid bigint, _accept text, _today text) OWNER TO postgres;

--
-- TOC entry 803 (class 1255 OID 23497)
-- Name: procupdatedonationnew(text, integer, integer, integer, text, bigint, double precision, double precision, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdatedonationnew(_code text, _plataforma integer, _uid integer, _status integer, _update text, _epin_id bigint, _gross_amount double precision, _net_amount double precision, _escrow_date text) RETURNS TABLE("_ID_" bigint)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_ID BIGINT = -1;
BEGIN
	SELECT index INTO _ID FROM pangya.pangya_donation_new WHERE code = _CODE AND plataforma = _PLATAFORMA;
	
	IF _CODE != '' AND _UPDATE IS NOT NULL AND _ID != -1 THEN

		UPDATE pangya.pangya_donation_new
			SET
				UID = _UID,
				status = _STATUS,
				update = _UPDATE::timestamp,
				epin_id = _EPIN_ID,
				gross_amount = _GROSS_AMOUNT,
				net_amount = _NET_AMOUNT,
				escrow = _ESCROW_DATE::timestamp
		WHERE
			index = _ID;

	END IF;
	
	RETURN QUERY SELECT _ID AS ID;
END;
$$;


ALTER FUNCTION pangya.procupdatedonationnew(_code text, _plataforma integer, _uid integer, _status integer, _update text, _epin_id bigint, _gross_amount double precision, _net_amount double precision, _escrow_date text) OWNER TO postgres;

--
-- TOC entry 804 (class 1255 OID 23498)
-- Name: procupdatedonationnewbygm(bigint, text, integer, integer, integer, text, bigint, double precision, double precision, text, text, text, integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdatedonationnewbygm(_id bigint, _code text, _plataforma integer, _uid integer, _status integer, _update text, _epin_id bigint, _gross_amount double precision, _net_amount double precision, _escrow_date text, _email text, _date text, _type integer, _reference text) RETURNS TABLE("_ID_" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _CODE != '' AND _UPDATE IS NOT NULL AND _ID != -1 THEN

		UPDATE pangya.pangya_donation_new
			SET
				code = _CODE,
				plataforma = _PLATAFORMA,
				UID = _UID,
				status = _STATUS,
				update = _UPDATE::timestamp,
				epin_id = _EPIN_ID,
				gross_amount = _GROSS_AMOUNT,
				net_amount = _NET_AMOUNT,
				escrow = _ESCROW_DATE::timestamp,
				reference = _REFERENCE,
				date = _DATE::timestamp,
				email = _EMAIL,
				type = _TYPE
		WHERE
			index = _ID;

	END IF;
	
	RETURN QUERY SELECT _ID AS ID;
END;
$$;


ALTER FUNCTION pangya.procupdatedonationnewbygm(_id bigint, _code text, _plataforma integer, _uid integer, _status integer, _update text, _epin_id bigint, _gross_amount double precision, _net_amount double precision, _escrow_date text, _email text, _date text, _type integer, _reference text) OWNER TO postgres;

--
-- TOC entry 805 (class 1255 OID 23499)
-- Name: procupdatefriendinfo(integer, integer, text, integer, integer, integer, integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdatefriendinfo(_iduser integer, _friend_uid integer, _friend_apelido text, _friend_unk1 integer, _friend_unk2 integer, _friend_unk3 integer, _friend_unk4 integer, _friend_unk5 integer, _friend_unk6 integer, _friend_flag1 integer, _friend_state integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDUSER > 0 AND _FRIEND_UID > 0 THEN
		UPDATE pangya.pangya_friend_list 
			SET  apelido = _FRIEND_APELIDO
				,unknown1 = _FRIEND_UNK1
				,unknown2 = _FRIEND_UNK2
				,unknown3 = _FRIEND_UNK3
				,unknown4 = _FRIEND_UNK4
				,unknown5 = _FRIEND_UNK5
				,unknown6 = _FRIEND_UNK6
				,flag1 = _FRIEND_FLAG1
				,state_flag = _FRIEND_STATE
		WHERE UID = _IDUSER AND uid_friend = _FRIEND_UID;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procupdatefriendinfo(_iduser integer, _friend_uid integer, _friend_apelido text, _friend_unk1 integer, _friend_unk2 integer, _friend_unk3 integer, _friend_unk4 integer, _friend_unk5 integer, _friend_unk6 integer, _friend_flag1 integer, _friend_state integer) OWNER TO postgres;

--
-- TOC entry 806 (class 1255 OID 23500)
-- Name: procupdategachajpticketplayer(integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdategachajpticketplayer(_uid integer, _ticket integer, _ticket_id integer, _ticket_sub integer, _ticket_sub_id integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _UID > 0 THEN
		-- Ticket
		UPDATE pangya.pangya_item_warehouse
		SET
			c0 = _TICKET
		WHERE UID = _UID AND item_id = _TICKET_ID;

		-- Ticket Sub
		UPDATE pangya.pangya_item_warehouse
		SET
			c0 = _TICKET_SUB
		WHERE UID = _UID AND item_id = _TICKET_SUB_ID;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procupdategachajpticketplayer(_uid integer, _ticket integer, _ticket_id integer, _ticket_sub integer, _ticket_sub_id integer) OWNER TO postgres;

--
-- TOC entry 807 (class 1255 OID 23501)
-- Name: procupdategoldentimeevent(bigint, integer, text, text, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdategoldentimeevent(_id bigint, _type integer, _begin text, _end text, _rate integer, _is_end integer) RETURNS TABLE("_ID_" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _ID > 0 AND _BEGIN IS NOT NULL THEN

		UPDATE pangya.pangya_golden_time_info
			SET
				type = _TYPE,
				begin = _BEGIN::date,
				"end" = _END::date,
				rate = _RATE,
				is_end = _IS_END
		WHERE
			index = _ID;

	END IF;
	
	RETURN QUERY SELECT _ID AS ID;
END;
$$;


ALTER FUNCTION pangya.procupdategoldentimeevent(_id bigint, _type integer, _begin text, _end text, _rate integer, _is_end integer) OWNER TO postgres;

--
-- TOC entry 808 (class 1255 OID 23502)
-- Name: procupdategoldentimeitem(bigint, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdategoldentimeitem(_id bigint, _typeid integer, _qntd integer, _qntd_time integer, _rate integer) RETURNS TABLE("_ID_" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _ID > 0 AND _TYPEID > 0 THEN

		UPDATE pangya.pangya_golden_time_item
			SET
				typeid = _TYPEID,
				qntd = _QNTD,
				qntd_time = _QNTD_TIME,
				rate = _RATE
		WHERE
			index = _ID;

	END IF;
	
	RETURN QUERY SELECT _ID AS ID;
END;
$$;


ALTER FUNCTION pangya.procupdategoldentimeitem(_id bigint, _typeid integer, _qntd integer, _qntd_time integer, _rate integer) OWNER TO postgres;

--
-- TOC entry 809 (class 1255 OID 23503)
-- Name: procupdategoldentimeround(bigint, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdategoldentimeround(_id bigint, _time text) RETURNS TABLE("_ID_" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _ID > 0 AND _TIME IS NOT NULL THEN

		UPDATE pangya.pangya_golden_time_round
			SET
				time = _TIME::time
		WHERE
			index = _ID;

	END IF;
	
	RETURN QUERY SELECT _ID AS ID;
END;
$$;


ALTER FUNCTION pangya.procupdategoldentimeround(_id bigint, _time text) OWNER TO postgres;

--
-- TOC entry 810 (class 1255 OID 23504)
-- Name: procupdateguildinfo(integer, integer, integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdateguildinfo(_guild_uid integer, _condition_level integer, _permition_join integer, _master_comment text) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _GUILD_UID > 0 THEN --AND @MASTER_COMMENT != N''

		UPDATE
			pangya.pangya_guild
				SET GUILD_CONDITION_LEVEL = _CONDITION_LEVEL,
					GUILD_PERMITION_JOIN = _PERMITION_JOIN,
					GUILD_NOTICE = _MASTER_COMMENT
		WHERE GUILD_UID = _GUILD_UID;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procupdateguildinfo(_guild_uid integer, _condition_level integer, _permition_join integer, _master_comment text) OWNER TO postgres;

--
-- TOC entry 811 (class 1255 OID 23505)
-- Name: procupdateguildintroimg(integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdateguildintroimg(_guild_uid integer, _intro_img text) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _GUILD_UID > 0 AND _INTRO_IMG != '' THEN
		
		UPDATE
			pangya.pangya_guild
				SET GUILD_INTRO_IMG = _INTRO_IMG
		WHERE GUILD_UID = _GUILD_UID;

		-- Insert Log
		INSERT INTO pangya.pangya_guild_intro_img_log(intro_img) VALUES(_INTRO_IMG);

	END IF;
END;
$$;


ALTER FUNCTION pangya.procupdateguildintroimg(_guild_uid integer, _intro_img text) OWNER TO postgres;

--
-- TOC entry 800 (class 1255 OID 23506)
-- Name: procupdateguildmemberpoints(integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdateguildmemberpoints(_guild_uid integer, _member_uid integer, _point integer, _pang integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _GUILD_UID > 0 AND _MEMBER_UID > 0 THEN

		UPDATE pangya_guild_member
			SET GUILD_POINT = GUILD_POINT + _POINT,
				GUILD_PANG = GUILD_PANG + _PANG
		WHERE GUILD_UID = _GUILD_UID AND MEMBER_UID = _MEMBER_UID;

	END IF;
END;
$$;


ALTER FUNCTION pangya.procupdateguildmemberpoints(_guild_uid integer, _member_uid integer, _point integer, _pang integer) OWNER TO postgres;

--
-- TOC entry 466 (class 1255 OID 23507)
-- Name: procupdateguildmessageintro(integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdateguildmessageintro(_guild_uid integer, _msg_intro text) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _GUILD_UID > 0 THEN --AND @MSG_INTRO != N''

		UPDATE
			pangya.pangya_guild
				SET GUILD_INFO = _MSG_INTRO
		WHERE GUILD_UID = _GUILD_UID;

	END IF;
END;
$$;


ALTER FUNCTION pangya.procupdateguildmessageintro(_guild_uid integer, _msg_intro text) OWNER TO postgres;

--
-- TOC entry 812 (class 1255 OID 23508)
-- Name: procupdateguildnewmarkidx(integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdateguildnewmarkidx(_guild_uid integer, _master integer, _new_mark_idx integer) RETURNS TABLE("_RET_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_RET INT = -1;
BEGIN
	IF _GUILD_UID > 0 AND _NEW_MARK_IDX > 0 AND _MASTER > 0 THEN

		IF (SELECT GUILD_UID FROM pangya.pangya_guild WHERE GUILD_UID = _GUILD_UID AND GUILD_LEADER = _MASTER) IS NOT NULL THEN
		DECLARE
			_PANG BIGINT = (SELECT pang FROM pangya.user_info WHERE UID = _MASTER);
		BEGIN

			-- Verifica se ele tem os 5k de pangs para fazer o pedido para trocar de guild mark
			IF _PANG IS NOT NULL AND _PANG >= 5000 THEN

				UPDATE
					pangya.pangya_guild
						SET GUILD_NEW_MARK_IDX = _NEW_MARK_IDX
				WHERE GUILD_UID = _GUILD_UID;

				-- Tira os 5k de pangs é o valor para trocar o guild mark
				UPDATE
					pangya.user_info
						SET Pang = _PANG - 5000
				WHERE UID = _MASTER;

				-- Sucesso
				_RET = 1;
			ELSE
				_RET = -2;	-- Não tem pangs suficiente para trocar o guild mark
			END IF;

		END;
		ELSE
			_RET = -3;	-- Player não é o master da Guild
		END IF;

	END IF;

	RETURN QUERY SELECT _RET AS RET;
END;
$$;


ALTER FUNCTION pangya.procupdateguildnewmarkidx(_guild_uid integer, _master integer, _new_mark_idx integer) OWNER TO postgres;

--
-- TOC entry 813 (class 1255 OID 23509)
-- Name: procupdateguildpoints(integer, bigint, bigint, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdateguildpoints(_uid integer, _point bigint, _pang bigint, _flag_win integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _UID > 0 THEN

		UPDATE pangya.pangya_guild
			SET GUILD_POINT = GUILD_POINT + _POINT,
				GUILD_PANG = GUILD_PANG + _PANG,
				GUILD_WIN = (CASE WHEN _FLAG_WIN = 0/*WIN*/ THEN GUILD_WIN + 1 ELSE GUILD_WIN END),
				GUILD_LOSE = (CASE WHEN _FLAG_WIN = 1/*LOSE*/ THEN GUILD_LOSE + 1 ELSE GUILD_LOSE END),
				GUILD_DRAW = (CASE WHEN _FLAG_WIN = 2/*DRAW*/ THEN GUILD_DRAW + 1 ELSE GUILD_DRAW END)
		WHERE GUILD_UID = _UID;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procupdateguildpoints(_uid integer, _point bigint, _pang bigint, _flag_win integer) OWNER TO postgres;

--
-- TOC entry 814 (class 1255 OID 23510)
-- Name: procupdateitembufftime(integer, bigint, integer, integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdateitembufftime(_iduser integer, _index bigint, _item_typeid integer, _tipo integer, _end_dt text) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDUSER > 0 AND _INDEX > 0 THEN
		UPDATE pangya.pangya_item_buff 
			SET typeid = _ITEM_TYPEID,
				tipo = _TIPO,
				end_date = COALESCE(_END_DT::timestamp, now())
		WHERE UID = _IDUSER AND index = _INDEX  AND use_yn = 1;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procupdateitembufftime(_iduser integer, _index bigint, _item_typeid integer, _tipo integer, _end_dt text) OWNER TO postgres;

--
-- TOC entry 815 (class 1255 OID 23511)
-- Name: procupdatelast5playergame(integer, integer, integer, text, text, integer, integer, text, text, integer, integer, text, text, integer, integer, text, text, integer, integer, text, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdatelast5playergame(_iduser integer, _uid_0 integer, _sex_0 integer, _id_0 text, _nick_0 text, _uid_1 integer, _sex_1 integer, _id_1 text, _nick_1 text, _uid_2 integer, _sex_2 integer, _id_2 text, _nick_2 text, _uid_3 integer, _sex_3 integer, _id_3 text, _nick_3 text, _uid_4 integer, _sex_4 integer, _id_4 text, _nick_4 text) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDUSER > 0 THEN
		UPDATE pangya.pangya_last_players_user
			SET UID_0 = _UID_0, SEX_0 = _SEX_0, ID_0 = _ID_0, NICK_0 = _NICK_0,
				UID_1 = _UID_1, SEX_1 = _SEX_1, ID_1 = _ID_1, NICK_1 = _NICK_1,
				UID_2 = _UID_2, SEX_2 = _SEX_0, ID_2 = _ID_2, NICK_2 = _NICK_2,
				UID_3 = _UID_3, SEX_3 = _SEX_3, ID_3 = _ID_3, NICK_3 = _NICK_3,
				UID_4 = _UID_4, SEX_4 = _SEX_4, ID_4 = _ID_4, NICK_4 = _NICK_4
		WHERE UID = _IDUSER;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procupdatelast5playergame(_iduser integer, _uid_0 integer, _sex_0 integer, _id_0 text, _nick_0 text, _uid_1 integer, _sex_1 integer, _id_1 text, _nick_1 text, _uid_2 integer, _sex_2 integer, _id_2 text, _nick_2 text, _uid_3 integer, _sex_3 integer, _id_3 text, _nick_3 text, _uid_4 integer, _sex_4 integer, _id_4 text, _nick_4 text) OWNER TO postgres;

--
-- TOC entry 816 (class 1255 OID 23512)
-- Name: procupdateloginrewardevent(bigint, text, integer, integer, integer, integer, integer, integer, integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdateloginrewardevent(_id bigint, _name text, _type integer, _days_to_gift integer, _n_times_gift integer, _item_typeid integer, _item_qntd integer, _item_qntd_time integer, _is_end integer, _end_date text) RETURNS TABLE("_ID_" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _ID > 0 AND _NAME != '' THEN

		UPDATE pangya.pangya_login_reward
			SET
				name = _NAME,
				type = _TYPE,
				days_to_gift = _DAYS_TO_GIFT,
				n_times_gift = _N_TIMES_GIFT,
				item_typeid = _ITEM_TYPEID,
				item_qntd = _ITEM_QNTD,
				item_qntd_time = _ITEM_QNTD_TIME,
				is_end = _IS_END,
				end_date = _END_DATE::timestamp
		WHERE
			index = _ID;
			
		RETURN QUERY SELECT _ID AS ID;
	ELSE
		RETURN QUERY SELECT -1::bigint AS ID;
	END IF;
	
END;
$$;


ALTER FUNCTION pangya.procupdateloginrewardevent(_id bigint, _name text, _type integer, _days_to_gift integer, _n_times_gift integer, _item_typeid integer, _item_qntd integer, _item_qntd_time integer, _is_end integer, _end_date text) OWNER TO postgres;

--
-- TOC entry 817 (class 1255 OID 23513)
-- Name: procupdateloginrewardplayer(bigint, integer, integer, integer, integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdateloginrewardplayer(_id bigint, _uid integer, _count_days integer, _count_seq integer, _is_clear integer, _update_date text) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _ID > 0 AND _UID > 0 THEN
		
		UPDATE pangya.pangya_login_reward_player
			SET
				COUNT_DAYS = _COUNT_DAYS,
				COUNT_SEQ = _COUNT_SEQ,
				IS_CLEAR = _IS_CLEAR,
				UPDATE_DATE = _UPDATE_DATE::timestamp
		WHERE
			index = _ID AND UID = _UID;
	END IF;
	
END;
$$;


ALTER FUNCTION pangya.procupdateloginrewardplayer(_id bigint, _uid integer, _count_days integer, _count_seq integer, _is_clear integer, _update_date text) OWNER TO postgres;

--
-- TOC entry 818 (class 1255 OID 23514)
-- Name: procupdatemapstatistics(integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, bigint, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdatemapstatistics(_iduser integer, _tipo integer, _course integer, _tacada integer, _putt integer, _hole integer, _fairway integer, _holein integer, _puttin integer, _total_score integer, _best_score integer, _best_pang bigint, _character_typeid integer, _event_score integer, _assist integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDUSER > 0 THEN
		IF (SELECT tacada FROM pangya.pangya_record WHERE UID = _IDUSER AND tipo = _TIPO AND course = _COURSE AND assist = _ASSIST) IS NULL THEN
			INSERT INTO pangya.pangya_record(uid, tipo, course, tacada, putt, hole, fairway, holein, puttin, total_score, best_score, best_pang, character_typeid, event_score, assist)
				VALUES(_IDUSER, _TIPO, _COURSE, _TACADA, _PUTT, _HOLE, _FAIRWAY, _HOLEIN, _PUTTIN, _TOTAL_SCORE, _BEST_SCORE, _BEST_PANG, _CHARACTER_TYPEID, _EVENT_SCORE, _ASSIST);
		ELSE
			UPDATE pangya.pangya_record 
				SET tacada = _TACADA,
					putt = _PUTT,
					hole = _HOLE,
					fairway = _FAIRWAY,
					holein = _HOLEIN,
					puttin = _PUTTIN,
					total_score = _TOTAL_SCORE,
					best_score = _BEST_SCORE,
					best_pang = _BEST_PANG,
					character_typeid = _CHARACTER_TYPEID,
					event_score = _EVENT_SCORE
				WHERE UID = _IDUSER AND tipo = _TIPO AND course = _COURSE AND assist = _ASSIST;

		END IF;
	END IF;
	
END;
$$;


ALTER FUNCTION pangya.procupdatemapstatistics(_iduser integer, _tipo integer, _course integer, _tacada integer, _putt integer, _hole integer, _fairway integer, _holein integer, _puttin integer, _total_score integer, _best_score integer, _best_pang bigint, _character_typeid integer, _event_score integer, _assist integer) OWNER TO postgres;

--
-- TOC entry 819 (class 1255 OID 23515)
-- Name: procupdatemascotinfo(integer, integer, integer, integer, integer, integer, integer, integer, integer, text, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdatemascotinfo(_iduser integer, _id integer, _typeid integer, _level integer, _exp integer, _flag integer, _tipo integer, _is_cash integer, _price integer, _message text, _end_dt text) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDUSER > 0 THEN
		UPDATE pangya.pangya_mascot_info
			SET mLevel = _LEVEL,
				mExp = _EXP,
				Flag = _FLAG,
				Tipo = _TIPO,
				IsCash = _IS_CASH,
				Price = _PRICE,
				Message = _MESSAGE,
				EndDate = _END_DT::timestamp
		WHERE UID = _IDUSER AND item_id = _ID AND typeid = _TYPEID;
	END IF;
	
END;
$$;


ALTER FUNCTION pangya.procupdatemascotinfo(_iduser integer, _id integer, _typeid integer, _level integer, _exp integer, _flag integer, _tipo integer, _is_cash integer, _price integer, _message text, _end_dt text) OWNER TO postgres;

--
-- TOC entry 820 (class 1255 OID 23516)
-- Name: procupdatemascottime(integer, integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdatemascottime(_iduser integer, _iditem integer, _end_date text) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	UPDATE pangya.pangya_mascot_info SET EndDate = _END_DATE::timestamp WHERE UID = _IDUSER AND item_id = _IDITEM;
END;
$$;


ALTER FUNCTION pangya.procupdatemascottime(_iduser integer, _iditem integer, _end_date text) OWNER TO postgres;

--
-- TOC entry 821 (class 1255 OID 23517)
-- Name: procupdatepapelshopconfig(integer, integer, integer, integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdatepapelshopconfig(_numero integer, _price_normal integer, _price_big integer, _limitted_per_day integer, _update_date text) RETURNS TABLE("_STATUS_" integer, "_NUMERO_" integer, "_PRICE_NORMAL_" bigint, "_PRICE_BIG_" bigint, "_LIMITTED_PER_DAY_" smallint, "_UPDATE_DATE_" timestamp without time zone)
    LANGUAGE plpgsql
    AS $$
BEGIN
	-- Block TABLE
	LOCK TABLE pangya.pangya_papel_shop_config IN ACCESS EXCLUSIVE MODE;

	IF (SELECT count(*) FROM pangya.pangya_papel_shop_config) > 0 THEN
		
		UPDATE pangya.pangya_papel_shop_config
			SET Numero = _NUMERO,
				Price_Normal = _PRICE_NORMAL,
				Price_Big = _PRICE_BIG,
				Limitted_YN = _LIMITTED_PER_DAY,
				Update_Date = _UPDATE_DATE::timestamp
		WHERE Update_Date IS NULL OR (CAST(now() AS DATE) - CAST(Update_Date AS DATE)) >= 1;

	ELSE
		INSERT INTO pangya.pangya_papel_shop_config(Numero, Price_Normal, Price_Big, Limitted_YN, Update_Date)
			VALUES(@NUMERO, _PRICE_NORMAL, _PRICE_BIG, _LIMITTED_PER_DAY, _UPDATE_DATE::timestamp);
	END IF;

	IF found THEN
		RETURN QUERY SELECT 1 AS OK, 0::int, 0::bigint, 0::bigint, 0::smallint, null::timestamp;
	ELSE
		RETURN QUERY SELECT 0 AS OK, * FROM pangya.pangya_papel_shop_config;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procupdatepapelshopconfig(_numero integer, _price_normal integer, _price_big integer, _limitted_per_day integer, _update_date text) OWNER TO postgres;

--
-- TOC entry 822 (class 1255 OID 23518)
-- Name: procupdatepapelshopinfo(integer, integer, integer, integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdatepapelshopinfo(_iduser integer, _current_cnt integer, _remain_cnt integer, _limit_cnt integer, _last_update text) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	-- Insert statements for procedure here
	IF _IDUSER > 0 THEN
		UPDATE pangya.pangya_papel_shop_info 
			SET current_cnt = _CURRENT_CNT, 
				remain_cnt = _REMAIN_CNT,
				limit_cnt = _LIMIT_CNT,
				last_update = _LAST_UPDATE::timestamp
		WHERE UID = _IDUSER;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procupdatepapelshopinfo(_iduser integer, _current_cnt integer, _remain_cnt integer, _limit_cnt integer, _last_update text) OWNER TO postgres;

--
-- TOC entry 823 (class 1255 OID 23519)
-- Name: procupdateplayerlocation(integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdateplayerlocation(_iduser integer, _channel integer, _lobby integer, _room integer, _place integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDUSER > 0 THEN
		UPDATE pangya.pangya_player_location
			SET channel = _CHANNEL,
				lobby = _LOBBY,
				room = _ROOM,
				place = _PLACE
		WHERE UID = _IDUSER;
	END IF;
	
END;
$$;


ALTER FUNCTION pangya.procupdateplayerlocation(_iduser integer, _channel integer, _lobby integer, _room integer, _place integer) OWNER TO postgres;

--
-- TOC entry 824 (class 1255 OID 23520)
-- Name: procupdatepremiumtickettime(integer, integer, integer, integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdatepremiumtickettime(_iduser integer, _iditem integer, _tempo integer, _c_0 integer, _c_1 integer, _c_2 integer, _c_3 integer, _c_4 integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDUSER > 0 AND _IDITEM > 0 THEN
		UPDATE pangya.pangya_item_warehouse
			SET
				EndDate = (now() + ('1 day'::interval * _TEMPO)),
				C0 = _C_0,
				C1 = _C_1,
				C2 = _C_2,
				C3 = _C_3,
				C4 = _C_4
		WHERE
			UID = _IDUSER AND item_id = _IDITEM;
	END IF;
	
END;
$$;


ALTER FUNCTION pangya.procupdatepremiumtickettime(_iduser integer, _iditem integer, _tempo integer, _c_0 integer, _c_1 integer, _c_2 integer, _c_3 integer, _c_4 integer) OWNER TO postgres;

--
-- TOC entry 825 (class 1255 OID 23521)
-- Name: procupdatequestuser(bigint, integer, integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdatequestuser(_iduser bigint, _quest_id integer, _countert_item_id integer, _date text) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	
	UPDATE pangya.pangya_quest SET counter_item_id = _COUNTERT_ITEM_ID, date = _DATE::timestamp  WHERE UID = _IDUSER AND id = _QUEST_ID;
END;
$$;


ALTER FUNCTION pangya.procupdatequestuser(_iduser bigint, _quest_id integer, _countert_item_id integer, _date text) OWNER TO postgres;

--
-- TOC entry 826 (class 1255 OID 23522)
-- Name: procupdaterateconfiginfo(integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdaterateconfiginfo(_server_uid integer, _grand_zodiac_event integer, _scratchy integer, _papel_shop_rare_item integer, _papel_shop_cookie_item integer, _treasure integer, _pang integer, _exp integer, _club_mastery integer, _chuva integer, _memorial_shop integer, _angel_event integer, _grand_prix_event integer, _golden_time_event integer, _login_reward_event integer, _bot_gm_event integer, _smart_calculator integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _SERVER_UID > 0 THEN

		IF (SELECT UID FROM pangya.pangya_config WHERE UID = _SERVER_UID) IS NOT NULL THEN -- Update
			UPDATE pangya.pangya_config
				SET GrandZodiacEventTime = _GRAND_ZODIAC_EVENT,
					ScratchyPorPointRate = _SCRATCHY,
					PapelShopRareItemRate = _PAPEL_SHOP_RARE_ITEM,
					PapelShopCookieItemRate = _PAPEL_SHOP_COOKIE_ITEM,
					TreasureRate = _TREASURE,
					PangRate = _PANG,
					ExpRate = _EXP,
					ClubMasteryRate = _CLUB_MASTERY,
					ChuvaRate = _CHUVA,
					MemorialShopRate = _MEMORIAL_SHOP,
					AngelEvent = _ANGEL_EVENT,
					GrandPrixEvent = _GRAND_PRIX_EVENT,
					GoldenTimeEvent = _GOLDEN_TIME_EVENT,
					LoginRewardEvent = _LOGIN_REWARD_EVENT,
					BotGMEvent = _BOT_GM_EVENT,
					SmartCalculator = _SMART_CALCULATOR
			WHERE UID = _SERVER_UID;
		ELSE
			-- Cria um novo server config rate
			INSERT INTO pangya.pangya_config(UID, GrandZodiacEventTime, ScratchyPorPointRate, PapelShopRareItemRate, PapelShopCookieItemRate, TreasureRate, PangRate, ExpRate, ClubMasteryRate, ChuvaRate, MemorialShopRate, AngelEvent, GrandPrixEvent, GoldenTimeEvent, LoginRewardEvent, BotGMEvent, SmartCalculator)
				VALUES(_SERVER_UID, _GRAND_ZODIAC_EVENT, _SCRATCHY, _PAPEL_SHOP_RARE_ITEM, _PAPEL_SHOP_COOKIE_ITEM, _TREASURE, _PANG, _EXP, _CLUB_MASTERY, _CHUVA, _MEMORIAL_SHOP, _ANGEL_EVENT, _GRAND_PRIX_EVENT, _GOLDEN_TIME_EVENT, _LOGIN_REWARD_EVENT, _BOT_GM_EVENT, _SMART_CALCULATOR);
		END IF;

	END IF;
END;
$$;


ALTER FUNCTION pangya.procupdaterateconfiginfo(_server_uid integer, _grand_zodiac_event integer, _scratchy integer, _papel_shop_rare_item integer, _papel_shop_cookie_item integer, _treasure integer, _pang integer, _exp integer, _club_mastery integer, _chuva integer, _memorial_shop integer, _angel_event integer, _grand_prix_event integer, _golden_time_event integer, _login_reward_event integer, _bot_gm_event integer, _smart_calculator integer) OWNER TO postgres;

--
-- TOC entry 827 (class 1255 OID 23523)
-- Name: procupdaterescuepasswordandid(integer, bigint); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdaterescuepasswordandid(_uid integer, _index bigint) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _UID > 0 AND _INDEX > 0 THEN
		
		UPDATE pangya.pangya_rescue_pwd_log SET state = 1 WHERE index = _INDEX AND UID = _UID;

	END IF;
	
END;
$$;


ALTER FUNCTION pangya.procupdaterescuepasswordandid(_uid integer, _index bigint) OWNER TO postgres;

--
-- TOC entry 828 (class 1255 OID 23524)
-- Name: procupdatetrofelnormal(integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdatetrofelnormal(_iduser integer, _ama_1_g integer, _ama_1_s integer, _ama_1_b integer, _ama_2_g integer, _ama_2_s integer, _ama_2_b integer, _ama_3_g integer, _ama_3_s integer, _ama_3_b integer, _ama_4_g integer, _ama_4_s integer, _ama_4_b integer, _ama_5_g integer, _ama_5_s integer, _ama_5_b integer, _ama_6_g integer, _ama_6_s integer, _ama_6_b integer, _pro_1_g integer, _pro_1_s integer, _pro_1_b integer, _pro_2_g integer, _pro_2_s integer, _pro_2_b integer, _pro_3_g integer, _pro_3_s integer, _pro_3_b integer, _pro_4_g integer, _pro_4_s integer, _pro_4_b integer, _pro_5_g integer, _pro_5_s integer, _pro_5_b integer, _pro_6_g integer, _pro_6_s integer, _pro_6_b integer, _pro_7_g integer, _pro_7_s integer, _pro_7_b integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDUSER > 0 THEN
		UPDATE pangya.trofel_stat
			SET AMA_1_G = _AMA_1_G,
				AMA_1_S = _AMA_1_S,
				AMA_1_B = _AMA_1_B,
				AMA_2_G = _AMA_2_G,
				AMA_2_S = _AMA_2_S,
				AMA_2_B = _AMA_2_B,
				AMA_3_G = _AMA_3_G,
				AMA_3_S = _AMA_3_S,
				AMA_3_B = _AMA_3_B,
				AMA_4_G = _AMA_4_G,
				AMA_4_S = _AMA_4_S,
				AMA_4_B = _AMA_4_B,
				AMA_5_G = _AMA_5_G,
				AMA_5_S = _AMA_5_S,
				AMA_5_B = _AMA_5_B,
				AMA_6_G = _AMA_6_G,
				AMA_6_S = _AMA_6_S,
				AMA_6_B = _AMA_6_B,
				PRO_1_G = _PRO_1_G,
				PRO_1_S = _PRO_1_S,
				PRO_1_B = _PRO_1_B,
				PRO_2_G = _PRO_2_G,
				PRO_2_S = _PRO_2_S,
				PRO_2_B = _PRO_2_B,
				PRO_3_G = _PRO_3_G,
				PRO_3_S = _PRO_3_S,
				PRO_3_B = _PRO_3_B,
				PRO_4_G = _PRO_4_G,
				PRO_4_S = _PRO_4_S,
				PRO_4_B = _PRO_4_B,
				PRO_5_G = _PRO_5_G,
				PRO_5_S = _PRO_5_S,
				PRO_5_B = _PRO_5_B,
				PRO_6_G = _PRO_6_G,
				PRO_6_S = _PRO_6_S,
				PRO_6_B = _PRO_6_B,
				PRO_7_G = _PRO_7_G,
				PRO_7_S = _PRO_7_S,
				PRO_7_B = _PRO_7_B
		WHERE UID = _IDUSER;
	END IF;
	
END;
$$;


ALTER FUNCTION pangya.procupdatetrofelnormal(_iduser integer, _ama_1_g integer, _ama_1_s integer, _ama_1_b integer, _ama_2_g integer, _ama_2_s integer, _ama_2_b integer, _ama_3_g integer, _ama_3_s integer, _ama_3_b integer, _ama_4_g integer, _ama_4_s integer, _ama_4_b integer, _ama_5_g integer, _ama_5_s integer, _ama_5_b integer, _ama_6_g integer, _ama_6_s integer, _ama_6_b integer, _pro_1_g integer, _pro_1_s integer, _pro_1_b integer, _pro_2_g integer, _pro_2_s integer, _pro_2_b integer, _pro_3_g integer, _pro_3_s integer, _pro_3_b integer, _pro_4_g integer, _pro_4_s integer, _pro_4_b integer, _pro_5_g integer, _pro_5_s integer, _pro_5_b integer, _pro_6_g integer, _pro_6_s integer, _pro_6_b integer, _pro_7_g integer, _pro_7_s integer, _pro_7_b integer) OWNER TO postgres;

--
-- TOC entry 829 (class 1255 OID 23525)
-- Name: procupdatetryhackingplayerluciaattendance(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdatetryhackingplayerluciaattendance(_uid integer, _try_hacking integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _UID > 0 THEN

		UPDATE pangya.pangya_lucia_attendance SET try_hacking_count = _TRY_HACKING WHERE UID = _UID;
	END IF;
	
END;
$$;


ALTER FUNCTION pangya.procupdatetryhackingplayerluciaattendance(_uid integer, _try_hacking integer) OWNER TO postgres;

--
-- TOC entry 831 (class 1255 OID 23526)
-- Name: procupdateucc(integer, integer, text, text, text, text, integer, integer, text, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdateucc(_iduser integer, _ucc_id integer, _ucc_idx text, _ucc_name text, _dt_draw text, _copier_nick text, _copier integer, _status integer, _use_yn text, _type integer) RETURNS TABLE("_ITEM_ID_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDUSER > 0 AND _UCC_ID > 0 THEN
		IF _TYPE = 2/*COPY UCC*/ THEN
		DECLARE
			_SEQ INT = COALESCE((SELECT COUNT(seq) as COUNT FROM pangya.tu_ucc WHERE UCCIDX = _UCC_IDX), 0);
		BEGIN
			
			_SEQ = CASE WHEN _SEQ = 0 THEN 1 ELSE _SEQ + 1 END;

			UPDATE pangya.tu_ucc
				SET USE_YN = _USE_YN
					 ,UCC_NAME = _UCC_NAME
					 ,DRAW_DT = _DT_DRAW::timestamp
					 ,UCCIDX = _UCC_IDX
					 ,COPIER_NICK = _COPIER_NICK
					 ,COPIER = _COPIER
					 ,STATUS = _STATUS
					 ,SEQ = _SEQ
			WHERE
				UID = _IDUSER
				AND item_id = _UCC_ID;

			RETURN QUERY SELECT _SEQ AS SEQ;
		END;
		ELSE /*SAVE TEMPORARIO E DEFINITIVO*/
			UPDATE pangya.tu_ucc
				SET USE_YN = _USE_YN
					 ,UCC_NAME = _UCC_NAME
					 ,DRAW_DT = _DT_DRAW::timestamp
					 ,UCCIDX = _UCC_IDX
					 ,COPIER_NICK = _COPIER_NICK
					 ,COPIER = _COPIER
					 ,STATUS = _STATUS
			WHERE
				UID = _IDUSER
				AND item_id = _UCC_ID;
	
			RETURN QUERY SELECT _UCC_ID AS ITEM_ID;
		END IF;
	ELSE
		RETURN QUERY SELECT _UCC_ID AS ITEM_ID;
	END IF;
	
END;
$$;


ALTER FUNCTION pangya.procupdateucc(_iduser integer, _ucc_id integer, _ucc_idx text, _ucc_name text, _dt_draw text, _copier_nick text, _copier integer, _status integer, _use_yn text, _type integer) OWNER TO postgres;

--
-- TOC entry 832 (class 1255 OID 23527)
-- Name: procupdateuserinfo(integer, numeric, numeric, numeric, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, bigint, bigint, bigint, bigint, bigint, bigint, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, bigint, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procupdateuserinfo(_iduser integer, _best_drive numeric, _best_long_putt numeric, _best_chipin numeric, _combo integer, _all_combo integer, _tacada integer, _putt integer, _tempo integer, _tempo_tacada integer, _acerto_pangya integer, _timeout integer, _ob integer, _total_distancia integer, _hole integer, _holein integer, _hio integer, _bunker integer, _fairway integer, _albatross integer, _mad_conduta integer, _puttin integer, _media_score integer, _best_score_0 integer, _best_score_1 integer, _best_score_2 integer, _best_score_3 integer, _best_score_4 integer, _best_pang_0 bigint, _best_pang_1 bigint, _best_pang_2 bigint, _best_pang_3 bigint, _best_pang_4 bigint, _sum_pang bigint, _event_flag integer, _jogado integer, _team_game integer, _team_win integer, _team_hole integer, _ladder_point integer, _ladder_hole integer, _ladder_win integer, _ladder_lose integer, _ladder_draw integer, _quitado integer, _skin_pang bigint, _skin_win integer, _skin_lose integer, _skin_run_hole integer, _skin_all_in_count integer, _nao_sei integer, _jogos_nao_sei integer, _event_value integer, _skin_strike_point integer, _max_jogos_nao_sei integer, _game_count_season integer, _total_pang_win_game integer, _medal_lucky integer, _medal_fast integer, _medal_best_drive integer, _medal_best_chipin integer, _medal_best_puttin integer, _medal_best_recovery integer, __16bit_nao_sei integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDUSER > 0 THEN
		UPDATE pangya.user_info 
			SET Max_distancia = _BEST_DRIVE
				,"Chip-in" = _BEST_CHIPIN
				,"Long-putt" = _BEST_LONG_PUTT
				,Combos = _COMBO
				,Todos_combos = _ALL_COMBO
				,Tacadas = _TACADA
				,Putt = _PUTT
				,Tempo = _TEMPO
				,"Tempo tacadas" = _TEMPO_TACADA
				,Acerto_pangya = _ACERTO_PANGYA
				,Timeout = _TIMEOUT
				,"O.B" = _OB
				,Total_distancia = _TOTAL_DISTANCIA
				,Holes = _HOLE
				,Holein = _HOLEIN
				,HIO = _HIO
				,Bunker = _BUNKER
				,Fairway = _FAIRWAY
				,Albatross = _ALBATROSS
				,MaConduta = _MAD_CONDUTA
				,Acerto_Putt = _PUTTIN
				/*,Xp = _EXP*/				 -- exp, level e pang, não atualiza aqui, por que ele tem os seus próprios
				/*,level = _LEVEL*/		 -- Esses aqui não atualiza que pode da erro depois
				/*,Pang = _PANG*/
				,Media_score = _MEDIA_SCORE
				,BestScore0 = _BEST_SCORE_0
				,BestScore1 = _BEST_SCORE_1
				,BestScore2 = _BEST_SCORE_2
				,BestScore3 = _BEST_SCORE_3
				,BestScore4 = _BEST_SCORE_4
				,MaxPang0 = _BEST_PANG_0
				,MaxPang1 = _BEST_PANG_1
				,MaxPang2 = _BEST_PANG_2
				,MaxPang3 = _BEST_PANG_3
				,MaxPang4 = _BEST_PANG_4
				,SumPang = _SUM_PANG
				,EventFlag = _EVENT_FLAG
				,Jogado = _JOGADO
				,TeamGames = _TEAM_GAME
				,TeamWin = _TEAM_WIN
				,Teamhole = _TEAM_HOLE
				,LadderPoint = _LADDER_POINT
				,LadderHole = _LADDER_HOLE
				,LadderWin = _LADDER_WIN
				,LadderLose = _LADDER_LOSE
				,LadderDraw = _LADDER_DRAW
				,Quitado = _QUITADO
				,SkinPang = _SKIN_PANG
				,SkinWin = _SKIN_WIN
				,SkinLose = _SKIN_LOSE
				,SkinRunHole = _SKIN_RUN_HOLE
				,SkinAllinCount = _SKIN_ALL_IN_COUNT
				,NaoSei = _NAO_SEI
				,JogosNaoSei = _JOGOS_NAO_SEI
				,EventValue = _EVENT_VALUE
				,SkinStrikePoint = _SKIN_STRIKE_POINT
				,MaxJogoNaoSei = _MAX_JOGOS_NAO_SEI
				,GameCountSeason = _GAME_COUNT_SEASON
				,total_pang_win_game = _TOTAL_PANG_WIN_GAME
				,lucky_medal = _MEDAL_LUCKY
				,fast_medal = _MEDAL_FAST
				,best_drive_medal = _MEDAL_BEST_DRIVE
				,best_chipin_medal = _MEDAL_BEST_CHIPIN
				,best_puttin_medal = _MEDAL_BEST_PUTTIN
				,best_recovery_medal = _MEDAL_BEST_RECOVERY
				,"16bit_naosei" = __16BIT_NAO_SEI
		WHERE UID = _IDUSER;
	END IF;
	
END;
$$;


ALTER FUNCTION pangya.procupdateuserinfo(_iduser integer, _best_drive numeric, _best_long_putt numeric, _best_chipin numeric, _combo integer, _all_combo integer, _tacada integer, _putt integer, _tempo integer, _tempo_tacada integer, _acerto_pangya integer, _timeout integer, _ob integer, _total_distancia integer, _hole integer, _holein integer, _hio integer, _bunker integer, _fairway integer, _albatross integer, _mad_conduta integer, _puttin integer, _media_score integer, _best_score_0 integer, _best_score_1 integer, _best_score_2 integer, _best_score_3 integer, _best_score_4 integer, _best_pang_0 bigint, _best_pang_1 bigint, _best_pang_2 bigint, _best_pang_3 bigint, _best_pang_4 bigint, _sum_pang bigint, _event_flag integer, _jogado integer, _team_game integer, _team_win integer, _team_hole integer, _ladder_point integer, _ladder_hole integer, _ladder_win integer, _ladder_lose integer, _ladder_draw integer, _quitado integer, _skin_pang bigint, _skin_win integer, _skin_lose integer, _skin_run_hole integer, _skin_all_in_count integer, _nao_sei integer, _jogos_nao_sei integer, _event_value integer, _skin_strike_point integer, _max_jogos_nao_sei integer, _game_count_season integer, _total_pang_win_game integer, _medal_lucky integer, _medal_fast integer, _medal_best_drive integer, _medal_best_chipin integer, _medal_best_puttin integer, _medal_best_recovery integer, __16bit_nao_sei integer) OWNER TO postgres;

--
-- TOC entry 833 (class 1255 OID 23529)
-- Name: procuseitembuff(integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procuseitembuff(_iduser integer, _item_typeid integer, _tipo integer, _percent integer, _tempo integer) RETURNS TABLE("_INDEX_" bigint)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_INDEX BIGINT = -1;
BEGIN
	IF _IDUSER > 0 AND _ITEM_TYPEID > 0 AND _TEMPO > 0 THEN
		INSERT INTO pangya.pangya_item_buff(UID, typeid, tipo, percent, reg_date, end_date, use_yn) 
			VALUES(_IDUSER, _ITEM_TYPEID, _TIPO, _PERCENT, now(), (now() + ('1 minute'::interval * _TEMPO)), 1)
		RETURNING index INTO _INDEX;
	END IF;

	RETURN QUERY SELECT _INDEX AS INDEX;
	
END;
$$;


ALTER FUNCTION pangya.procuseitembuff(_iduser integer, _item_typeid integer, _tipo integer, _percent integer, _tempo integer) OWNER TO postgres;

--
-- TOC entry 834 (class 1255 OID 23530)
-- Name: procverificaauthkeys(integer, text, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procverificaauthkeys(_iduser integer, _key_in text, _opt_in integer) RETURNS TABLE("_RET_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_RET int = 0;
BEGIN
	IF _OPT_IN = 0 THEN
		IF 
			(CASE 
				WHEN 
					(
						SELECT authkey.AuthKey
						FROM pangya.authkey
						WHERE authkey.UID = _IDUSER
					) = _key_in THEN 0
				WHEN 
					(
						SELECT authkey.AuthKey
						FROM pangya.authkey
						WHERE authkey.UID = _IDUSER
					) > _key_in THEN 1
				WHEN 
					(
						SELECT authkey.AuthKey
						FROM pangya.authkey
						WHERE authkey.UID = _IDUSER
					) < _key_in THEN -1
				ELSE NULL::int
			END) = 0 THEN
			_RET = 1;
		END IF;
	ELSE 
		IF 
			(CASE 
				WHEN 
				(
					SELECT authkey.AuthKey_login
					FROM pangya.authkey
					WHERE authkey.UID = _IDUSER
				) = _key_in THEN 0
				WHEN 
				(
					SELECT authkey.AuthKey_login
					FROM pangya.authkey
					WHERE authkey.UID = _IDUSER
				) > _key_in THEN 1
				WHEN 
				(
					SELECT authkey.AuthKey_login
					FROM pangya.authkey
					WHERE authkey.UID = _IDUSER
				) < _key_in THEN -1
				ELSE NULL::int
			END) = 0 THEN
			_RET = 1;
		END IF;
	END IF;

	IF _RET = 1 THEN
		UPDATE pangya.authkey
			SET 
				valid = 1
		WHERE authkey.UID = _IDUSER;
	END IF;

	RETURN QUERY SELECT _RET AS RET;
	
END;
$$;


ALTER FUNCTION pangya.procverificaauthkeys(_iduser integer, _key_in text, _opt_in integer) OWNER TO postgres;

--
-- TOC entry 835 (class 1255 OID 23531)
-- Name: procverificacoursecubecoin(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procverificacoursecubecoin(_course_in integer) RETURNS TABLE(_active_ smallint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT pangya_course_cube_coin_temporada.active
	FROM pangya.pangya_course_cube_coin_temporada
	WHERE pangya_course_cube_coin_temporada.course = _course_in;
	
END;
$$;


ALTER FUNCTION pangya.procverificacoursecubecoin(_course_in integer) OWNER TO postgres;

--
-- TOC entry 836 (class 1255 OID 23532)
-- Name: procverificanick(integer, text, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procverificanick(_iduser integer, _nick_user text, _opt integer) RETURNS TABLE("_UID_" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _OPT = 0 THEN
		RETURN QUERY SELECT account.UID
		FROM pangya.account
		WHERE account.NICK = _NICK_USER;
	ELSIF _OPT = 1 THEN

		IF _NICK_USER LIKE '% %' THEN
			RETURN QUERY SELECT -3::bigint AS UID;
		END IF;
	END IF;
	
END;
$$;


ALTER FUNCTION pangya.procverificanick(_iduser integer, _nick_user text, _opt integer) OWNER TO postgres;

--
-- TOC entry 837 (class 1255 OID 23533)
-- Name: procverifyid(text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procverifyid(_id text) RETURNS TABLE("_UID" bigint, "ID" character varying)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _ID != '' THEN
		RETURN QUERY SELECT uid, id FROM pangya.account WHERE convert_to(id, 'latin1') = convert_to(_ID, 'latin1');
	END IF;
END;
$$;


ALTER FUNCTION pangya.procverifyid(_id text) OWNER TO postgres;

--
-- TOC entry 838 (class 1255 OID 23534)
-- Name: procverifyip(integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procverifyip(_iduser integer, _ip text) RETURNS TABLE(_uid integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDUSER > 0 AND _IP != '' THEN
		RETURN QUERY SELECT uid FROM pangya.pangya_player_ip WHERE ip = _IP AND uid = _IDUSER AND block_beta = 0;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procverifyip(_iduser integer, _ip text) OWNER TO postgres;

--
-- TOC entry 839 (class 1255 OID 23535)
-- Name: procverifynickname(text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procverifynickname(_nickname text) RETURNS TABLE(_uid bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _NICKNAME != N'' THEN
		RETURN QUERY SELECT uid FROM pangya.account WHERE nick = _NICKNAME;
	END IF;
END;
$$;


ALTER FUNCTION pangya.procverifynickname(_nickname text) OWNER TO postgres;

--
-- TOC entry 840 (class 1255 OID 23536)
-- Name: procverifypass(integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procverifypass(_iduser integer, _pass text) RETURNS TABLE("_UID" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT uid FROM pangya.account WHERE UID = _IDUSER AND PASSWORD = UPPER(_PASS);
END;
$$;


ALTER FUNCTION pangya.procverifypass(_iduser integer, _pass text) OWNER TO postgres;

--
-- TOC entry 830 (class 1255 OID 23537)
-- Name: procwriteguildbbs(integer, integer, text, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procwriteguildbbs(_player_uid integer, _type integer, _title text, _text text) RETURNS TABLE("_SEQ_" bigint)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_SEQ BIGINT = -1;
BEGIN
	IF _PLAYER_UID > 0 AND _TITLE != '' AND _TEXT != '' THEN
		
		INSERT INTO pangya.pangya_guild_bbs(OWNER_UID, TITLE, TEXT, TYPE)
		VALUES(_PLAYER_UID, _TITLE, _TEXT, _TYPE)
		RETURNING seq INTO _SEQ;

	END IF;
	
	RETURN QUERY SELECT _SEQ AS SEQ;
END;
$$;


ALTER FUNCTION pangya.procwriteguildbbs(_player_uid integer, _type integer, _title text, _text text) OWNER TO postgres;

--
-- TOC entry 841 (class 1255 OID 23538)
-- Name: procwriteguildbbsreply(integer, bigint, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procwriteguildbbsreply(_owner_uid integer, _bbs_seq bigint, _text text) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _OWNER_UID > 0 AND _BBS_SEQ > 0 AND _TEXT != '' THEN

		INSERT INTO pangya.pangya_guild_bbs_res(OWNER_UID, BBS_SEQ, TEXT)
		VALUES(_OWNER_UID, _BBS_SEQ, _TEXT);

	END IF;
END;
$$;


ALTER FUNCTION pangya.procwriteguildbbsreply(_owner_uid integer, _bbs_seq bigint, _text text) OWNER TO postgres;

--
-- TOC entry 842 (class 1255 OID 23539)
-- Name: procwriteguildnotice(integer, integer, text, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procwriteguildnotice(_guild_uid integer, _player_uid integer, _title text, _text text) RETURNS TABLE("_SEQ_" bigint)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_SEQ BIGINT = -1;
BEGIN
	IF _GUILD_UID > 0 AND _PLAYER_UID > 0 AND _TITLE != '' AND _TEXT != '' THEN

		INSERT INTO pangya.pangya_guild_notice(GUILD_UID, OWNER_UID, TITLE, TEXT)
		VALUES(_GUILD_UID, _PLAYER_UID, _TITLE, _TEXT)
		RETURNING seq INTO _SEQ;

	END IF;
	
	RETURN QUERY SELECT _SEQ AS SEQ;
END;
$$;


ALTER FUNCTION pangya.procwriteguildnotice(_guild_uid integer, _player_uid integer, _title text, _text text) OWNER TO postgres;

--
-- TOC entry 843 (class 1255 OID 23540)
-- Name: procwriteguildprivatebbs(integer, integer, text, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procwriteguildprivatebbs(_guild_uid integer, _player_uid integer, _title text, _text text) RETURNS TABLE("_SEQ_" bigint)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_SEQ BIGINT = -1;
BEGIN
	IF _GUILD_UID > 0 AND _PLAYER_UID > 0 AND _TITLE != '' AND _TEXT != '' THEN

		INSERT INTO pangya.pangya_guild_private_bbs(GUILD_UID, OWNER_UID, TITLE, TEXT)
		VALUES(_GUILD_UID, _PLAYER_UID, _TITLE, _TEXT)
		RETURNING seq INTO _SEQ;

	END IF;
	
	RETURN QUERY SELECT _SEQ AS SEQ;
END;
$$;


ALTER FUNCTION pangya.procwriteguildprivatebbs(_guild_uid integer, _player_uid integer, _title text, _text text) OWNER TO postgres;

--
-- TOC entry 844 (class 1255 OID 23541)
-- Name: procwriteguildprivatebbsreply(integer, bigint, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.procwriteguildprivatebbsreply(_player_uid integer, _bbs_seq bigint, _text text) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _PLAYER_UID > 0 AND _BBS_SEQ > 0 THEN
		
		INSERT INTO pangya.pangya_guild_private_bbs_res(GUILD_BBS_SEQ, OWNER_UID, TEXT)
		VALUES(_BBS_SEQ, _PLAYER_UID, _TEXT);

	END IF;
END;
$$;


ALTER FUNCTION pangya.procwriteguildprivatebbsreply(_player_uid integer, _bbs_seq bigint, _text text) OWNER TO postgres;

--
-- TOC entry 845 (class 1255 OID 23542)
-- Name: usp_add_furniture(integer, integer, integer, real, real, real, real); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.usp_add_furniture(_iduser integer, _iditem integer, _itemtypeid integer, _f_x real, _f_y real, _f_z real, _f_r real) RETURNS TABLE("_ITEM_ID_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDITEM <= 0 THEN
        SELECT MYROOM_ID INTO _IDITEM
        FROM pangya.td_room_data
        WHERE UID = _IDUSER AND TYPEID = _ITEMTYPEID;
    END IF;

    IF NOT _IDITEM > 0 THEN

        INSERT INTO pangya.td_room_data(
            UID, 
            TYPEID, 
            POS_X, 
            POS_Y, 
            POS_Z, 
            POS_R)
        VALUES (
                _IDUSER, 
                _ITEMTYPEID, 
                _F_X, 
                _F_Y, 
                _F_Z, 
                _F_R);

        SELECT max(MYROOM_ID) INTO _IDITEM
        FROM pangya.td_room_data
        WHERE UID = _IDUSER;

    END IF;

    RETURN QUERY SELECT _IDITEM AS ITEM_ID;
END;
$$;


ALTER FUNCTION pangya.usp_add_furniture(_iduser integer, _iditem integer, _itemtypeid integer, _f_x real, _f_y real, _f_z real, _f_r real) OWNER TO postgres;

--
-- TOC entry 846 (class 1255 OID 23543)
-- Name: usp_add_item(integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, real, real, real, real); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.usp_add_item(_iduser integer, _giftflag integer, _purchase_in integer, _iditem integer, _itemtypeid integer, _typeflag integer, _typeflag_iff integer, _tempo integer, _c_0 integer, _c_1 integer, _c_2 integer, _c_3 integer, _c_4 integer, _x real, _y real, _z real, _r real) RETURNS TABLE(_item_id_ integer, "_UCCIDX_" character varying, "_SD_SEQ_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_NUMERO INT;
	_CONTADOR INT;
	_CHARACTER INT;
	_PERIODO INT;
	_CADDIE_TYPE INT;
	_ITEM_TYPE INT;
	_COR_CABELO INT;
	_Gift_flg INT;
BEGIN
	-- Group ID
	SELECT public.GroupID(_ITEMTYPEID) INTO _NUMERO;

	IF _NUMERO = 4 THEN  --#Character
	BEGIN
		IF _IDITEM <= 0 THEN
			SELECT item_id INTO _IDITEM
			FROM pangya.pangya_character_information 
			WHERE UID = _IDUSER AND typeid = _ITEMTYPEID;
		END IF;

		IF _IDITEM > 0 THEN
				UPDATE pangya_user_equip SET character_id = _IDITEM WHERE UID = _IDUSER;
		ELSE
			perform pangya.FuncInsertPartsCharacter(_IDUSER, _ITEMTYPEID, 0, 0);

			SELECT MAX(item_id) INTO _IDITEM FROM pangya_character_information WHERE UID = _IDUSER AND typeid = _ITEMTYPEID;

			_CONTADOR = 0;
			WHILE _CONTADOR < 24 LOOP
				INSERT INTO td_char_equip_s4(UID, CHAR_ITEMID, IN_DATE, EQUIP_NUM, USE_YN)
						VALUES(_IDUSER, _IDITEM, now(), _CONTADOR + 1, 'Y');
				_CONTADOR = _CONTADOR + 1;
			END LOOP;

			UPDATE pangya_user_equip SET character_id = _IDITEM WHERE UID = _IDUSER;
		END IF;

		RETURN QUERY SELECT _IDITEM AS ITEM_ID, 'unknown'::varchar(8), 0;
	END;
	ELSIF _NUMERO = 8 THEN --#Parts
	BEGIN

		IF _TYPEFLAG_IFF = 8 OR _TYPEFLAG_IFF = 9 THEN
		BEGIN
			IF _IDITEM <= 0 THEN
				perform pangya.USP_ADD_UCC(_IDUSER, _ITEMTYPEID, _GIFTFLAG, _C_0, _C_1, _C_2, _C_3, _C_4, _PURCHASE_IN, _TYPEFLAG_IFF, _TEMPO);
			ELSIF _IDITEM > 0 AND _C_0 >= 0 THEN
			BEGIN
				UPDATE pangya.pangya_item_warehouse SET valid = 1, Gift_flag = 0 
				WHERE UID = _IDUSER AND item_id = _IDITEM;

				RETURN QUERY SELECT _IDITEM AS ITEM_ID, 
						(SELECT UCCIDX FROM tu_ucc WHERE UID = _IDUSER AND ITEM_ID = _IDITEM) AS UCCIDX,
						(SELECT SEQ FROM tu_ucc WHERE UID = _IDUSER AND ITEM_ID = _IDITEM) AS SD_SEQ;
			END;
			ELSIF _IDITEM > 0 AND _C_0 < 0 THEN
				UPDATE pangya.pangya_item_warehouse SET valid = 0
				WHERE UID = _IDUSER AND item_id = _IDITEM;

				RETURN QUERY SELECT _IDITEM AS ITEM_ID, 
						(SELECT UCCIDX FROM tu_ucc WHERE UID = _IDUSER AND ITEM_ID = _IDITEM) AS UCCIDX,
						(SELECT SEQ FROM tu_ucc WHERE UID = _IDUSER AND ITEM_ID = _IDITEM) AS SD_SEQ;
			END IF;
		END;
		ELSE
			IF _IDITEM > 0 THEN --#Extender o Rental
			BEGIN
				IF _TEMPO > 0 AND (SELECT flag FROM pangya.pangya_item_warehouse WHERE UID = _IDUSER AND typeid = _ITEMTYPEID LIMIT 1) = 96 THEN
					UPDATE pangya.pangya_item_warehouse SET EndDate = (now() + (_TEMPO * '1 day'::interval)) 
					WHERE UID = _IDUSER AND item_id = _IDITEM;
				ELSIF _C_0 < 0 THEN
					UPDATE pangya.pangya_item_warehouse SET valid = 0 
					WHERE UID = _IDUSER AND item_id = _IDITEM;
				ELSIF _C_0 > 0 THEN
					UPDATE pangya.pangya_item_warehouse SET valid = 1, Gift_flag = _GIFTFLAG 
					WHERE UID = _IDUSER AND item_id = _IDITEM;
				END IF;
			END;
			ELSE
				IF _TEMPO > 0 THEN
					INSERT INTO pangya.pangya_item_warehouse(UID, typeid, valid, regdate, Gift_flag, flag, Purchase, ItemType, EndDate, c0, c1, c2, c3, c4)
								VALUES(_IDUSER, _ITEMTYPEID, 1, now(), _GIFTFLAG, _TYPEFLAG, 0, 2, (now() + (_TEMPO * '1 day'::interval)), _C_0, _C_1, _C_2, _C_3, _C_4);
				ELSE
					INSERT INTO pangya.pangya_item_warehouse(UID, typeid, valid, regdate, enddate, Gift_flag, flag, Purchase, ItemType, c0, c1, c2, c3, c4)
								VALUES(_IDUSER, _ITEMTYPEID, 1, getdate(), getdate(), _GIFTFLAG, _TYPEFLAG, 0, 2, _C_0, _C_1, _C_2, _C_3, _C_4);
				END IF;

				SELECT MAX(ITEM_ID) INTO _IDITEM FROM pangya_item_warehouse WHERE UID = _IDUSER;
			END IF;

			RETURN QUERY SELECT _IDITEM AS ITEM_ID, 'unknown'::varchar(8), 0;
		END IF;
	END;
	ELSIF _NUMERO = 16 THEN --#ClubSet
	BEGIN
		IF _IDITEM <= 0 THEN
			SELECT item_id INTO _IDITEM
			FROM pangya.pangya_item_warehouse
			WHERE UID = _IDUSER AND typeid = _ITEMTYPEID;
			
			SELECT Gift_flag INTO _Gift_flg
			FROM pangya_item_warehouse
			WHERE UID = _IDUSER AND typeid = _ITEMTYPEID;
		END IF;

		IF _IDITEM > 0 THEN
		BEGIN
			IF _C_0 < 0 THEN	--# excluiu, vendeu ou deu"gift"
			BEGIN
				UPDATE pangya.pangya_item_warehouse 
					SET  valid = 0,
							ClubSet_WorkShop_Flag = 0,
							ClubSet_WorkShop_C0 = 0,
							ClubSet_WorkShop_C1 = 0,
							ClubSet_WorkShop_C2 = 0,
							ClubSet_WorkShop_C3 = 0,
							ClubSet_WorkShop_C4 = 0,
							Mastery_Pts = 0,
							Recovery_Pts = 0,
							Level = 0,
							Up = 0,
							Total_Mastery_Pts = 0,
							Mastery_Gasto = 0
				WHERE UID = _IDUSER AND item_id = _IDITEM;
				
				--# Zera o clubset_enchantment pq excluiu a taquiera
				IF (SELECT Gift_flag FROM pangya_item_warehouse WHERE UID = _IDUSER AND item_id = _IDITEM) = 0 THEN
					UPDATE pangya_clubset_enchant SET pang = 0, c0 = 0, c1 = 0, c2 = 0, c3 = 0, c4 = 0
						WHERE UID = _IDUSER AND item_id = _IDITEM;
				END IF;
			END;
			ELSIF _C_0 > 0 AND _Gift_flg = 1 THEN --# gift
				UPDATE pangya_item_warehouse SET valid = 1, Gift_flag = _GIFTFLAG
				WHERE UID = _IDUSER AND item_id = _IDITEM;
			ELSIF _C_0 > 0 THEN	--# Comprou a taqueira de novo (antes ele excluiu, vendeu ou deu"gift")
				UPDATE pangya_item_warehouse SET valid = 1, C0 = _C_0, C1 = _C_1, C2 = _C_2, C3 = _C_3, C4 = _C_4
				WHERE UID = _IDUSER AND item_id = _IDITEM;
				
				--# Zera o clubset_enchantment pq comprou a taquiera
				UPDATE pangya_clubset_enchant SET pang = 0, c0 = 0, c1 = 0, c2 = 0, c3 = 0, c4 = 0
				WHERE UID = _IDUSER AND item_id = _IDITEM;
			END IF;
		END;
		ELSE
			INSERT INTO pangya_item_warehouse(UID, typeid, valid, regdate, Gift_flag, flag, Purchase, ItemType, c0, c1, c2, c3, c4)
						VALUES(_IDUSER, _ITEMTYPEID, 1, now(), _GIFTFLAG, _TYPEFLAG, 0, 2, _C_0, _C_1, _C_2, _C_3, _C_4);

			SELECT MAX(item_id) INTO _IDITEM FROM pangya_item_warehouse WHERE UID = _IDUSER AND typeid = _ITEMTYPEID;

			INSERT INTO pangya_clubset_enchant(uid, item_id) VALUES(_IDUSER, _IDITEM);
		END IF;

		RETURN QUERY SELECT _IDITEM AS ITEM_ID, 'unknown'::varchar(8), 0;
	END;
	ELSIF _NUMERO = 20 THEN --#Ball
	BEGIN
		IF _IDITEM <= 0 THEN
			SELECT item_id INTO _IDITEM
			FROM pangya_item_warehouse
			WHERE UID = _IDUSER AND typeid = _ITEMTYPEID;
		END IF;

		IF _IDITEM > 0 THEN
		DECLARE 
				_ATIVO INT;
				_C0_QNTD INT;
		BEGIN

			SELECT valid INTO _ATIVO FROM pangya_item_warehouse WHERE UID = _IDUSER AND item_id = _IDITEM;

			IF _ATIVO = 1 THEN
			BEGIN
				UPDATE pangya_item_warehouse SET C0 = C0 + _C_0 WHERE UID = _IDUSER AND item_id = _IDITEM;

				SELECT C0 INTO _C0_QNTD FROM pangya_item_warehouse WHERE UID = _IDUSER AND item_id = _IDITEM;

				IF _C0_QNTD <= 0 THEN
					UPDATE pangya_item_warehouse SET valid = 0, C0 = 0 WHERE UID = _IDUSER AND item_id = _IDITEM;
				END IF;
			END;
			ELSE
				UPDATE pangya_item_warehouse SET C0 = C0 + _C_0, valid = 1 WHERE UID = _IDUSER AND item_id = _IDITEM;
			END IF;
		END;
		ELSE
			INSERT INTO pangya_item_warehouse(UID, typeid, valid, regdate, Gift_flag, flag, C0, Purchase, ItemType)
					VALUES(_IDUSER, _ITEMTYPEID, 1, now(), _GIFTFLAG, _TYPEFLAG, _C_0, 0, 2);

			SELECT MAX(ITEM_ID) INTO _IDITEM FROM pangya_item_warehouse WHERE UID = _IDUSER;
		END IF;

		RETURN QUERY SELECT _IDITEM AS ITEM_ID, 'unknown'::varchar(8), 0;
	END;
	ELSIF _NUMERO = 24 THEN --#Item _ATIVO
	BEGIN
		IF _IDITEM <= 0 THEN
			SELECT item_id INTO _IDITEM
			FROM pangya_item_warehouse
			WHERE UID = _IDUSER AND typeid = _ITEMTYPEID;
		END IF;

		IF _IDITEM > 0 THEN
		DECLARE 
			_ATIVO INT;
			_C0_QNTD INT;
		BEGIN

			SELECT valid INTO _ATIVO FROM pangya_item_warehouse WHERE UID = _IDUSER AND item_id = _IDITEM;
			SELECT C0 INTO _C0_QNTD FROM pangya_item_warehouse WHERE UID = _IDUSER AND item_id = _IDITEM;
			
			IF _ATIVO = 1 AND _C0_QNTD > 0 THEN
			BEGIN
				UPDATE pangya_item_warehouse SET C0 = C0 + _C_0 WHERE UID = _IDUSER AND item_id = _IDITEM;

				SELECT C0 INTO _C0_QNTD FROM pangya_item_warehouse WHERE UID = _IDUSER AND item_id = _IDITEM;

				IF _C0_QNTD <= 0 THEN
					UPDATE pangya_item_warehouse SET valid = 0, C0 = 0 WHERE UID = _IDUSER AND item_id = _IDITEM;
				END IF;
			END;
			ELSIF _C_0 > 0 THEN  --# ADD um novo item, pq o anterior tinha sido exclui "valid = 0"
			BEGIN
				IF _TEMPO > 0 THEN
					UPDATE pangya_item_warehouse 
						SET C0 = _C_0, C1 = _C_1, C2 = _C_2, C3 = _C_3, C4 = _C_4,
							flag = _TYPEFLAG, valid = 1, EndDate = (now() + (_TEMPO * '1 day'::interval))
					WHERE UID = _IDUSER AND item_id = _IDITEM;
				ELSE
					UPDATE pangya_item_warehouse SET C0 = _C_0, C1 = _C_1, C2 = _C_2, C3 = _C_3, C4 = _C_4, flag = _TYPEFLAG, valid = 1
					WHERE UID = _IDUSER AND item_id = _IDITEM;
				END IF;
			END;
			ELSE
				UPDATE pangya_item_warehouse SET valid = 0, C0 = 0 WHERE UID = _IDUSER AND item_id = _IDITEM;
			END IF;
		END;
		ELSE
			IF _TEMPO > 0 THEN
			BEGIN
				INSERT INTO pangya_item_warehouse(UID, typeid, valid, regdate, EndDate, Gift_flag, flag, C0, C1, C2, C3, C4, Purchase, ItemType)
						VALUES(_IDUSER, _ITEMTYPEID, 1, now(), (now() + (_TEMPO * '1 day'::interval)), _GIFTFLAG, _TYPEFLAG, _C_0, _C_1, _C_2, _C_3, _C_4, 0, 2);
				
				SELECT MAX(ITEM_ID) INTO _IDITEM FROM pangya_item_warehouse WHERE UID = _IDUSER;
			END;
			ELSE
				INSERT INTO pangya_item_warehouse(UID, typeid, valid, regdate, Gift_flag, flag, C0, C1, C2, C3, C4, Purchase, ItemType)
						VALUES(_IDUSER, _ITEMTYPEID, 1, now(), _GIFTFLAG, _TYPEFLAG, _C_0, _C_1, _C_2, _C_3, _C_4, 0, 2);
			
				SELECT MAX(ITEM_ID) INTO _IDITEM FROM pangya_item_warehouse WHERE UID = _IDUSER;
			END IF;
		END IF;

		RETURN QUERY SELECT _IDITEM AS ITEM_ID, 'unknown'::varchar(8), 0;

	END;
	ELSIF _NUMERO = 26 THEN --#Item Passivo
	BEGIN
		IF _IDITEM <= 0 AND _ITEMTYPEID != x'1A000042'::int THEN /*Ticket Report tem q ser outro item*/
			SELECT item_id INTO _IDITEM
			FROM pangya_item_warehouse
			WHERE UID = _IDUSER AND typeid = _ITEMTYPEID;
		END IF;

		IF _ITEMTYPEID = x'1A000010'::int THEN --# Pouch de Pangya transferi para os pangs do usuario
			UPDATE user_info SET Pang = Pang + _C_0 WHERE UID = _IDUSER;
		ELSIF _ITEMTYPEID = x'1A00015D'::int THEN --# Pouch de EXP tranfere para o User Info
			perform pangya.USP_EXP_PLAYER_ATT(_IDUSER, _C_0);
		ELSE
			IF _IDITEM > 0 THEN
			DECLARE 
				_ATIVO INT;
				_C0_QNTD INT;
			BEGIN

				SELECT valid INTO _ATIVO FROM pangya_item_warehouse WHERE UID = _IDUSER AND item_id = _IDITEM;
				SELECT c0 INTO _C0_QNTD FROM pangya_item_warehouse WHERE UID = _IDUSER AND item_id = _IDITEM;
				
				IF _ATIVO = 1 AND _C0_QNTD > 0 THEN
				BEGIN
					UPDATE pangya_item_warehouse SET C0 = C0 + _C_0 WHERE UID = _IDUSER AND item_id = _IDITEM;

					SELECT C0 INTO _C0_QNTD FROM pangya_item_warehouse WHERE UID = _IDUSER AND item_id = _IDITEM;

					IF _C0_QNTD <= 0 THEN
						UPDATE pangya_item_warehouse SET valid = 0, C0 = 0 WHERE UID = _IDUSER AND item_id = _IDITEM;
					END IF;
				
				END;
				ELSIF _C_0 > 0 THEN  --# ADD um novo item, pq o anterior tinha sido exclui "valid = 0"
				BEGIN
					IF _TEMPO > 0 THEN
					BEGIN
						UPDATE pangya_item_warehouse 
							SET C0 = _C_0, C1 = _C_1, C2 = _C_2, C3 = _C_3, C4 = _C_4,
								flag = _TYPEFLAG, valid = 1, EndDate = (now() + (_TEMPO * '1 day'::interval))
						WHERE UID = _IDUSER AND item_id = _IDITEM;
					END;
					ELSE
						UPDATE pangya_item_warehouse SET C0 = _C_0, C1 = _C_1, C2 = _C_2, C3 = _C_3, C4 = _C_4, flag = _TYPEFLAG, valid = 1
						WHERE UID = _IDUSER AND item_id = _IDITEM;
					END IF;
				END;
				ELSE
					UPDATE pangya_item_warehouse SET valid = 0, C0 = 0 WHERE UID = _IDUSER AND item_id = _IDITEM;
				END IF;
			END;
			ELSE
				IF _TEMPO > 0 THEN
				BEGIN
					INSERT INTO pangya_item_warehouse(UID, typeid, valid, regdate, EndDate, Gift_flag, flag, C0, C1, C2, C3, C4, Purchase, ItemType)
							VALUES(_IDUSER, _ITEMTYPEID, 1, now(), (now() + (_TEMPO * '1 day'::interval)), _GIFTFLAG, _TYPEFLAG, _C_0, _C_1, _C_2, _C_3, _C_4, 0, 2);

					SELECT MAX(ITEM_ID) INTO _IDITEM FROM pangya_item_warehouse WHERE UID = _IDUSER;
				END;
				ELSE
					INSERT INTO pangya_item_warehouse(UID, typeid, valid, regdate, Gift_flag, flag, C0, C1, C2, C3, C4, Purchase, ItemType)
							VALUES(_IDUSER, _ITEMTYPEID, 1, now(), _GIFTFLAG, _TYPEFLAG, _C_0, _C_1, _C_2, _C_3, _C_4, 0, 2);

					SELECT MAX(ITEM_ID) INTO _IDITEM FROM pangya_item_warehouse WHERE UID = _IDUSER;
				END IF;
			END IF;

			RETURN QUERY SELECT _IDITEM AS ITEM_ID, 'unknown'::varchar(8), 0;
		END IF;

		RETURN QUERY SELECT 0 AS ITEM_ID, 'unknown'::varchar(8), 0;
	END;
	ELSIF _NUMERO = 27 THEN --#Item Passivo
	BEGIN
		IF _IDITEM <= 0 THEN
			SELECT item_id INTO _IDITEM
			FROM pangya_item_warehouse
			WHERE UID = _IDUSER AND typeid = _ITEMTYPEID;
		END IF;

		IF _ITEMTYPEID = x'1A000010'::int THEN --# Pouch de Pangya transferi para os pangs do usuario
			UPDATE user_info SET Pang = Pang + _C_0 WHERE UID = _IDUSER;
		ELSIF _ITEMTYPEID = x'1A00015D'::int THEN --# Pouch de EXP tranfere para o User Info
			perform pangya.USP_EXP_PLAYER_ATT(_IDUSER, _C_0);
		ELSE
			IF _IDITEM > 0 THEN
			DECLARE 
				_ATIVO INT;
				_C0_QNTD INT;
			BEGIN
			
				SELECT valid INTO _ATIVO FROM pangya_item_warehouse WHERE UID = _IDUSER AND item_id = _IDITEM;
				SELECT C0 INTO _C0_QNTD FROM pangya_item_warehouse WHERE UID = _IDUSER AND item_id = _IDITEM;
			
				IF _ATIVO = 1 AND _C0_QNTD > 0 THEN
				BEGIN
					UPDATE pangya_item_warehouse SET C0 = C0 + _C_0 WHERE UID = _IDUSER AND item_id = _IDITEM;

					SELECT C0 INTO _C0_QNTD FROM pangya_item_warehouse WHERE UID = _IDUSER AND item_id = _IDITEM;

					IF _C0_QNTD <= 0 THEN
						UPDATE pangya_item_warehouse SET valid = 0, C0 = 0 WHERE UID = _IDUSER AND item_id = _IDITEM;
					END IF;
				END;
				ELSIF _C_0 > 0 THEN --# ADD um novo item, pq o anterior tinha sido exclui "valid = 0"
				BEGIN
					IF _TEMPO > 0 THEN
					BEGIN
						UPDATE pangya_item_warehouse 
							SET C0 = _C_0, C1 = _C_1, C2 = _C_2, C3 = _C_3, C4 = _C_4, 
								flag = _TYPEFLAG, valid = 1, EndDate = (now() + (_TEMPO * '1 day'::interval))
						WHERE UID = _IDUSER AND item_id = _IDITEM;
					END;
					ELSE
						UPDATE pangya_item_warehouse SET C0 = _C_0, C1 = _C_1, C2 = _C_2, C3 = _C_3, C4 = _C_4, flag = _TYPEFLAG, valid = 1
						WHERE UID = _IDUSER AND item_id = _IDITEM;
					END IF;
				END;
				ELSE
					UPDATE pangya_item_warehouse SET valid = 0, C0 = 0 WHERE UID = _IDUSER AND item_id = _IDITEM;
				END IF;
			END;
			ELSE
				IF _TEMPO > 0 THEN
				BEGIN
					INSERT INTO pangya_item_warehouse(UID, typeid, valid, regdate, EndDate, Gift_flag, flag, C0, C1, C2, C3, C4, Purchase, ItemType)
						VALUES(_IDUSER, _ITEMTYPEID, 1, now(), (now() + (_TEMPO * '1 day'::interval)), _GIFTFLAG, _TYPEFLAG, _C_0, _C_1, _C_2, _C_3, _C_4, 0, 2);

					SELECT MAX(ITEM_ID) INTO _IDITEM FROM pangya_item_warehouse WHERE UID = _IDUSER;
				END;
				ELSE
					INSERT INTO pangya_item_warehouse(UID, typeid, valid, regdate, Gift_flag, flag, C0, C1, C2, C3, C4, Purchase, ItemType)
						VALUES(_IDUSER, _ITEMTYPEID, 1, now(), _GIFTFLAG, _TYPEFLAG, _C_0, _C_1, _C_2, _C_3, _C_4, 0, 2);

					SELECT MAX(ITEM_ID) INTO _IDITEM FROM pangya_item_warehouse WHERE UID = _IDUSER;
				END IF;
			END IF;

			RETURN QUERY SELECT _IDITEM AS ITEM_ID, 'unknown'::varchar(8), 0;
		END IF;

		RETURN QUERY SELECT 0 AS ITEM_ID, 'unknown'::varchar(8), 0;
	END;
	ELSIF _NUMERO = 28 THEN --#Caddie
	BEGIN
		IF _IDITEM <= 0 THEN
			SELECT item_id INTO _IDITEM
			FROM pangya_caddie_information
			WHERE UID = _IDUSER AND typeid = _ITEMTYPEID;
		END IF;

		IF _IDITEM > 0 THEN
		BEGIN
			IF _C_0 < 0 THEN
			BEGIN
				UPDATE pangya_caddie_information SET valid = 0 
				WHERE UID = _IDUSER AND item_id = _IDITEM;
			END;
			ELSIF _C_0 > 0 THEN
				UPDATE pangya_caddie_information SET valid = 1
				WHERE UID = _IDUSER AND item_id = _IDITEM;
			END IF;
		END;
		ELSE
			_PERIODO = CASE WHEN _TEMPO = 0 THEN 1 ELSE 2 END;

			INSERT INTO pangya_caddie_information(UID, typeid, gift_flag, regDate, Period, EndDate, RentFlag, Purchase, CheckEnd)
						VALUES(_IDUSER, _ITEMTYPEID, _GIFTFLAG, now(), _TEMPO, (now() + (_TEMPO * '1 day'::interval)), _PERIODO, 0, 1);

			SELECT MAX(item_id) INTO _IDITEM FROM pangya_caddie_information WHERE UID = _IDUSER AND typeid = _ITEMTYPEID;
		END IF;

		RETURN QUERY SELECT _IDITEM AS ITEM_ID, 'unknown'::varchar(8), 0;
	END;
	ELSIF _NUMERO = 32 THEN --#Caddie Item Antigos
	BEGIN
		SELECT ((_ITEMTYPEID & x'0FFF0000'::int) >> 16) / 32 INTO _CADDIE_TYPE;
		SELECT (_CADDIE_TYPE + x'1C000000'::int) + ((_ITEMTYPEID & x'000F0000'::int) >> 16) INTO _CADDIE_TYPE;

		IF _IDITEM <= 0 THEN
			SELECT item_id INTO _IDITEM FROM pangya_caddie_information 
			WHERE UID = _IDUSER AND typeid = _CADDIE_TYPE;
		END IF;
		
		IF _IDITEM > 0 THEN
			SELECT COUNT(*) INTO _CONTADOR FROM pangya_caddie_information 
			WHERE UID = _IDUSER AND (typeid = _CADDIE_TYPE AND parts_typeid = _ITEMTYPEID);
			
			IF _CONTADOR > 0 THEN
				UPDATE pangya_caddie_information SET parts_typeid = _ITEMTYPEID, parts_EndDate = CASE WHEN parts_EndDate <= now() THEN (now() + (_TEMPO * '1 day'::interval)) ELSE (parts_EndDate + (_TEMPO * '1 day'::interval)) END
				WHERE UID = _IDUSER AND item_id = _IDITEM;
			ELSE
				UPDATE pangya_caddie_information SET parts_typeid = _ITEMTYPEID, parts_EndDate = (now() + (_TEMPO * '1 day'::interval))
				WHERE UID = _IDUSER AND item_id = _IDITEM;
			END IF;
		END IF;

		RETURN QUERY SELECT _IDITEM AS ITEM_ID, 'unknown'::varchar(8), 0;
	END;
	ELSIF _NUMERO = 33 THEN --#Caddie Item Antigos
	BEGIN
		SELECT ((_ITEMTYPEID & x'0FFF0000'::int) >> 16) / 32 INTO _CADDIE_TYPE;
		SELECT (_CADDIE_TYPE + x'1C000000'::int) + ((_ITEMTYPEID & x'000F0000'::int) >> 16) INTO _CADDIE_TYPE;

		IF _IDITEM <= 0 THEN
			SELECT item_id INTO _IDITEM FROM pangya_caddie_information 
			WHERE UID = _IDUSER AND typeid = _CADDIE_TYPE;
		END IF;

		IF _IDITEM > 0 THEN
			SELECT COUNT(*) INTO _CONTADOR FROM pangya_caddie_information 
			WHERE UID = _IDUSER AND (typeid = _CADDIE_TYPE AND parts_typeid = _ITEMTYPEID);

			IF _CONTADOR > 0 THEN
				UPDATE pangya_caddie_information SET parts_typeid = _ITEMTYPEID, parts_EndDate = CASE WHEN parts_EndDate <= now() THEN (now() + (_TEMPO * '1 day'::interval)) ELSE (parts_EndDate + (_TEMPO * '1 day'::interval)) END
				WHERE UID = _IDUSER AND item_id = _IDITEM;
			ELSE
				UPDATE pangya_caddie_information SET parts_typeid = _ITEMTYPEID, parts_EndDate = (now() + (_TEMPO * '1 day'::interval))
				WHERE UID = _IDUSER AND item_id = _IDITEM;
			END IF;
		END IF;

		RETURN QUERY SELECT _IDITEM AS ITEM_ID, 'unknown'::varchar(8), 0;
	END;
	ELSIF _NUMERO = 34 THEN --#Caddie Item Novos
	BEGIN
		SELECT ((_ITEMTYPEID & x'0FFF0000'::int) >> 16) / 32 INTO _CADDIE_TYPE;
		SELECT (_CADDIE_TYPE + x'1C000000'::int) + ((_ITEMTYPEID & x'000F0000'::int) >> 16) INTO _CADDIE_TYPE;

		IF _IDITEM <= 0 THEN
			SELECT item_id INTO _IDITEM FROM pangya_caddie_information 
			WHERE UID = _IDUSER AND typeid = _CADDIE_TYPE;
		END IF;
		
		IF _IDITEM > 0 THEN
			SELECT COUNT(*) INTO _CONTADOR FROM pangya_caddie_information 
			WHERE UID = _IDUSER AND (typeid = _CADDIE_TYPE AND parts_typeid = _ITEMTYPEID);

			IF _CONTADOR > 0 THEN
				UPDATE pangya_caddie_information SET parts_typeid = _ITEMTYPEID, parts_EndDate = CASE WHEN parts_EndDate <= now() THEN (now() + (_TEMPO * '1 day'::interval)) ELSE (parts_EndDate + (_TEMPO * '1 day'::interval)) END
				WHERE UID = _IDUSER AND item_id = _IDITEM;
			ELSE
				UPDATE pangya_caddie_information SET parts_typeid = _ITEMTYPEID, parts_EndDate = (now() + (_TEMPO * '1 day'::interval))
				WHERE UID = _IDUSER AND item_id = _IDITEM;
			END IF;
		END IF;

		RETURN QUERY SELECT _IDITEM AS ITEM_ID, 'unknown'::varchar(8), 0;
	END;
	ELSIF _NUMERO = 35 THEN --#Caddie Item Antigos(Novos)
	BEGIN
		SELECT ((_ITEMTYPEID & x'0FFF0000'::int) >> 16) / 32 INTO _CADDIE_TYPE;
		SELECT (_CADDIE_TYPE + x'1C000000'::int) + ((_ITEMTYPEID & x'000F0000'::int) >> 16) INTO _CADDIE_TYPE;

		IF _IDITEM <= 0 THEN
			SELECT item_id INTO _IDITEM FROM pangya_caddie_information 
			WHERE UID = _IDUSER AND typeid = _CADDIE_TYPE;
		END IF;

		IF _IDITEM > 0 THEN
			SELECT COUNT(*) INTO _CONTADOR FROM pangya_caddie_information 
			WHERE UID = _IDUSER AND (typeid = _CADDIE_TYPE AND parts_typeid = _ITEMTYPEID);

			IF _CONTADOR > 0 THEN
				UPDATE pangya_caddie_information SET parts_typeid = _ITEMTYPEID, parts_EndDate = CASE WHEN parts_EndDate <= now() THEN (now() + (_TEMPO * '1 day'::interval)) ELSE (parts_EndDate + (_TEMPO * '1 day'::interval)) END
				WHERE UID = _IDUSER AND item_id = _IDITEM;
			ELSE
				UPDATE pangya_caddie_information SET parts_typeid = _ITEMTYPEID, parts_EndDate = (now() + (_TEMPO * '1 day'::interval))
				WHERE UID = _IDUSER AND item_id = _IDITEM;
			END IF;
		END IF;

		RETURN QUERY SELECT _IDITEM AS ITEM_ID, 'unknown'::varchar(8), 0;
	END;
	ELSIF _NUMERO = 45 THEN --# Trofel Especial
	BEGIN
		IF (SELECT item_id FROM pangya_trofel_especial WHERE UID = _IDUSER AND typeid = _ITEMTYPEID) IS NULL THEN
			INSERT INTO pangya_trofel_especial(UID, TYPEID, QNTD) VALUES(_IDUSER, _ITEMTYPEID, 1);
		ELSE
			UPDATE pangya_trofel_especial SET qntd = qntd + 1 WHERE UID = _IDUSER AND typeid = _ITEMTYPEID;
		END IF;
		
		RETURN QUERY SELECT ITEM_ID, 'unknown'::varchar(8), 0 FROM pangya_trofel_especial WHERE UID = _IDUSER AND typeid = _ITEMTYPEID;
	END;
	ELSIF _NUMERO = 47 THEN --# Trofel Grand Prix
	BEGIN
		IF (SELECT item_id FROM pangya_trofel_grandprix WHERE UID = _IDUSER AND typeid = _ITEMTYPEID) IS NULL THEN
			INSERT INTO pangya_trofel_grandprix(UID, TYPEID, QNTD) VALUES(_IDUSER, _ITEMTYPEID, 1);
		ELSE
			UPDATE pangya_trofel_grandprix SET qntd = qntd + 1 WHERE UID = _IDUSER AND typeid = _ITEMTYPEID;
		END IF;
		
		RETURN QUERY SELECT ITEM_ID, 'unknown'::varchar(8), 0 FROM pangya_trofel_grandprix WHERE UID = _IDUSER AND typeid = _ITEMTYPEID;
	END;
	ELSIF _NUMERO = 60 THEN --# HairStyle
	BEGIN
		_ITEM_TYPE = x'4000000'::int | _C_1;

		UPDATE pangya_character_information SET default_hair = _C_0 
		WHERE UID = _IDUSER AND typeid = _ITEM_TYPE;
		
		RETURN QUERY SELECT 0 AS ITEM_ID, 'unknown'::varchar(8), 0;
	END;
	ELSIF _NUMERO = 62 THEN --# HairStyle 2
	BEGIN
		_ITEM_TYPE = x'4000000'::int | _C_1;

		UPDATE pangya_character_information SET default_hair = _C_0 
		WHERE UID = _IDUSER AND typeid = _ITEM_TYPE;

		RETURN QUERY SELECT 0 AS ITEM_ID, 'unknown'::varchar(8), 0;
	END;
	ELSIF _NUMERO = 64 THEN --#Mascot
	BEGIN
		IF _IDITEM <= 0 THEN
			IF (SELECT item_id FROM pangya_mascot_info WHERE UID = _IDUSER AND typeid = _ITEMTYPEID AND valid = 0) IS NOT NULL THEN
			BEGIN
				SELECT item_id INTO _IDITEM
				FROM pangya_mascot_info
				WHERE UID = _IDUSER AND typeid = _ITEMTYPEID AND valid = 0;

				_C_0 = 1;
			END;
			ELSE
				SELECT item_id INTO _IDITEM
				FROM pangya_mascot_info
				WHERE UID = _IDUSER AND typeid = _ITEMTYPEID AND valid = 1;
			END IF;
		END IF;

		IF _IDITEM > 0 THEN
		BEGIN
			IF _TEMPO > 0 THEN
			BEGIN
				IF (SELECT EndDate FROM pangya_mascot_info WHERE UID = _IDUSER AND item_id = _IDITEM) <= now() THEN --# novo mascot, msm que já tinha, mas _TEMPO acabou
					UPDATE pangya_mascot_info SET Period = Period + _TEMPO, EndDate = (now() + (_TEMPO * '1 day'::interval)), message = 'Pangya'
					WHERE UID = _IDUSER AND item_id = _IDITEM;
				ELSE
					UPDATE pangya_mascot_info SET Period = Period + _TEMPO, EndDate = (now() + (_TEMPO * '1 day'::interval))
					WHERE UID = _IDUSER AND item_id = _IDITEM;
				END IF;
			END;
			ELSIF _C_0 < 0 THEN
				UPDATE pangya_mascot_info SET valid = 0
				WHERE UID = _IDUSER AND item_id = _IDITEM;
			ELSIF _C_0 > 0 THEN
				UPDATE pangya_mascot_info SET valid = 1, message = 'Pangya'
				WHERE UID = _IDUSER AND item_id = _IDITEM;
			END IF;
		END;
		ELSE
			IF _TEMPO = 0 THEN --# Mascot Sem Limit de _TEMPO
				INSERT INTO pangya_mascot_info(UID, typeid, Flag, Tipo, RegDate, Period, EndDate, IsCash, Valid)
						VALUES(_IDUSER, _ITEMTYPEID, 0, 0, now(), _TEMPO, (now() + (_TEMPO * '1 day'::interval)), 1, 1);
			ELSE	--# Mascot com limit de _TEMPO
				INSERT INTO pangya_mascot_info(UID, typeid, Flag, Tipo, RegDate, Period, EndDate, IsCash, Valid)
						VALUES(_IDUSER, _ITEMTYPEID, 0, 1, now(), _TEMPO, (now() + (_TEMPO * '1 day'::interval)), 0, 1);
			END IF;

			SELECT MAX(item_id) INTO _IDITEM FROM pangya_mascot_info WHERE UID = _IDUSER AND typeid = _ITEMTYPEID;
		END IF;

		RETURN QUERY SELECT _IDITEM AS ITEM_ID, 'unknown'::varchar(8), 0;
	END;
	ELSIF _NUMERO = 56 THEN --# Skiin (Background, Frame, Sticker, Slot)
	BEGIN
		IF _IDITEM <= 0 THEN
			SELECT item_id INTO _IDITEM FROM pangya_item_warehouse 
			WHERE UID = _IDUSER AND typeid = _ITEMTYPEID;
		END IF;

		IF _IDITEM > 0 THEN
		BEGIN
			IF _TEMPO > 0 THEN								--#flag 64 pq os anyigos estava errado isso é só para concerta
				UPDATE pangya_item_warehouse SET valid = 1, flag = 64, EndDate = CASE WHEN EndDate <= now() THEN (now() + (_TEMPO * '1 day'::interval)) ELSE (EndDate + (_TEMPO * '1 day'::interval)) END
				WHERE UID = _IDUSER AND item_id = _IDITEM;
			ELSIF _C_0 < 0 THEN
				UPDATE pangya_item_warehouse SET valid = 0
				WHERE UID = _IDUSER AND item_id = _IDITEM;
			ELSIF _C_0 > 0 THEN
				UPDATE pangya_item_warehouse SET valid = 1
				WHERE UID = _IDUSER AND item_id = _IDITEM;
			END IF;
		END;
		ELSE
			INSERT INTO pangya_item_warehouse(UID, typeid, valid, regdate, Gift_flag, flag, C0, Purchase, ItemType, EndDate)
					VALUES(_IDUSER, _ITEMTYPEID, 1, now(), _GIFTFLAG, _TYPEFLAG, 0, 0, 2, (now() + (_TEMPO * '1 day'::interval)));

			SELECT MAX(ITEM_ID) INTO _IDITEM FROM pangya_item_warehouse WHERE UID = _IDUSER;
		END IF;

		RETURN QUERY SELECT _IDITEM AS ITEM_ID, 'unknown'::varchar(8), 0;
	END;
	ELSIF _NUMERO = 57 THEN --# Cutin e Título
	BEGIN
		IF _IDITEM <= 0 THEN
			SELECT item_id INTO _IDITEM FROM pangya_item_warehouse 
			WHERE UID = _IDUSER AND typeid = _ITEMTYPEID;
		END IF;

		IF _IDITEM > 0 THEN
		BEGIN
			IF _TEMPO > 0 THEN								--#flag 64 pq os anyigos estava errado isso é só para concerta
				UPDATE pangya_item_warehouse SET valid = 1, flag = 64, EndDate = CASE WHEN EndDate <= now() THEN (now() + (_TEMPO * '1 day'::interval)) ELSE (EndDate + (_TEMPO * '1 day'::interval))END
				WHERE UID = _IDUSER AND item_id = _IDITEM;
			ELSIF _C_0 < 0 THEN
				UPDATE pangya_item_warehouse SET valid = 0
				WHERE UID = _IDUSER AND item_id = _IDITEM;
			ELSIF _C_0 > 0 THEN
				UPDATE pangya_item_warehouse SET valid = 1
				WHERE UID = _IDUSER AND item_id = _IDITEM;
			END IF;
		END;
		ELSE
			INSERT INTO pangya_item_warehouse(UID, typeid, valid, regdate, Gift_flag, flag, C0, Purchase, ItemType)
					VALUES(_IDUSER, _ITEMTYPEID, 1, now(), _GIFTFLAG, _TYPEFLAG, 0, 0, 2);

			SELECT MAX(ITEM_ID) INTO _IDITEM FROM pangya_item_warehouse WHERE UID = _IDUSER;
		END IF;
		
		RETURN QUERY SELECT _IDITEM AS ITEM_ID, 'unknown'::varchar(8), 0;
	END;
	ELSIF _NUMERO = 72 THEN --#Furniture - Chama a procedure do Furniture
	BEGIN
		perform pangya.USP_ADD_FURNITURE(_IDUSER, _IDITEM, _ITEMTYPEID, _X, _Y, _Z, _R);
	END;
	ELSIF _NUMERO = 108 THEN --# 0x6C COUNTER ITEM_TYPE
	BEGIN
		IF _IDITEM > 0 AND _C_0 < 0 AND _C_1 != 0 THEN
			UPDATE count_item SET count_num_item = 0, Data_Sec = 0 WHERE UID = _IDUSER AND count_id = _IDITEM;
            RETURN QUERY SELECT _IDITEM as ITEM_ID, 'unknown'::varchar(8), 0;
        ELSIF _IDITEM > 0 AND _C_0 < 0 AND _C_1 = 0 THEN
			DELETE FROM count_item WHERE UID = _IDUSER AND count_id = _IDITEM;
            RETURN QUERY SELECT _IDITEM as ITEM_ID, 'unknown'::varchar(8), 0;
		ELSIF _IDITEM > 0 AND _C_0 > 0 THEN --# att o countador do count_item
            perform pangya.USP_ATT_COUNTER_ITEM(_IDUSER, _ITEMTYPEID, _IDITEM, _C_0, _C_3, _C_4);
            RETURN QUERY SELECT _IDITEM as ITEM_ID, 'unknown'::varchar(8), 0;
		ELSIF _IDITEM > 0 AND _C_0 = 0 AND _C_1 != 0 THEN
			DELETE FROM count_item WHERE UID = _IDUSER AND count_id = _IDITEM;
            RETURN QUERY SELECT _IDITEM as ITEM_ID, 'unknown'::varchar(8), 0;
		ELSIF _IDITEM > 0 AND _C_0 = 0 AND _C_1 = 0 THEN
			UPDATE count_item SET count_num_item = _C_0, Data_Sec = 0 WHERE UID = _IDUSER AND count_id = _IDITEM;
            RETURN QUERY SELECT _IDITEM as ITEM_ID, 'unknown'::varchar(8), 0;
        ELSE
			RETURN QUERY SELECT count_id AS ITEM_ID, 'unknown'::varchar(8), 0 FROM count_item WHERE UID = _IDUSER AND TypeID = _ITEMTYPEID;
		END IF;
	END;
	ELSIF _NUMERO = 112 THEN --#Aux Part
	BEGIN
		IF _IDITEM <= 0 THEN
			SELECT item_id INTO _IDITEM
			FROM pangya_item_warehouse
			WHERE UID = _IDUSER AND typeid = _ITEMTYPEID;
		END IF;
		
		IF _IDITEM > 0 THEN
		DECLARE 
			_ATIVO INT;
			_C0_QNTD INT;
		BEGIN

			SELECT valid INTO _ATIVO FROM pangya_item_warehouse WHERE UID = _IDUSER AND item_id = _IDITEM;

			IF _ATIVO = 1 THEN
			BEGIN
				UPDATE pangya_item_warehouse SET C0 = C0 + _C_0 WHERE UID = _IDUSER AND item_id = _IDITEM;

				SELECT C0 INTO _C0_QNTD FROM pangya_item_warehouse WHERE UID = _IDUSER AND item_id = _IDITEM;

				IF _C0_QNTD <= 0 THEN
					UPDATE pangya_item_warehouse SET valid = 0, C0 = 0 WHERE UID = _IDUSER AND item_id = _IDITEM;
				END IF;
			END;
			ELSE
				UPDATE pangya_item_warehouse SET C0 = C0 + _C_0, valid = 1 WHERE UID = _IDUSER AND item_id = _IDITEM;
			END IF;
		END;
		ELSE
			INSERT INTO pangya_item_warehouse(UID, typeid, valid, regdate, Gift_flag, flag, C0, Purchase, ItemType)
					VALUES(_IDUSER, _ITEMTYPEID, 1, now(), _GIFTFLAG, _TYPEFLAG, _C_0, 0, 2);

			SELECT MAX(ITEM_ID) INTO _IDITEM FROM pangya_item_warehouse WHERE UID = _IDUSER;
		END IF;
		
		RETURN QUERY SELECT _IDITEM AS ITEM_ID, 'unknown'::varchar(8), 0;
	END;
	ELSIF _NUMERO = 120 THEN --# Daily quest achievement
    BEGIN
		RETURN QUERY SELECT ID_ACHIEVEMENT AS ITEM_ID, 'unknown'::varchar(8), 0 FROM achievement_tipo WHERE UID = _IDUSER AND TypeID = _ITEMTYPEID AND OPTION = 1;
    END;
	ELSIF _NUMERO = 124 THEN --#Pangya Card Pack #1, #2, #3, #4
	BEGIN
		IF _IDITEM <= 0 THEN
			SELECT card_itemid INTO _IDITEM FROM pangya_card 
			WHERE UID = _IDUSER AND card_typeid = _ITEMTYPEID;
		END IF;

		IF _IDITEM > 0 THEN
		DECLARE 
				_ATIVO_C CHAR;
				_C0_QNTD INT;
		BEGIN
			SELECT USE_YN INTO _ATIVO_C FROM pangya_card WHERE UID = _IDUSER AND card_itemid = _IDITEM;

			IF _ATIVO_C = 'N' THEN
			BEGIN
				UPDATE pangya_card SET QNTD = QNTD + _C_0 WHERE UID = _IDUSER AND card_itemid = _IDITEM;

				SELECT QNTD INTO _C0_QNTD FROM pangya_card WHERE UID = _IDUSER AND card_itemid = _IDITEM;

				IF _C0_QNTD <= 0 THEN
					UPDATE pangya_card SET USE_YN = 'Y', QNTD = 0 WHERE UID = _IDUSER AND card_itemid = _IDITEM;
				END IF;
			END;
			ELSE
				UPDATE pangya_card SET QNTD = QNTD + _C_0, USE_YN = 'N' WHERE UID = _IDUSER AND card_itemid = _IDITEM;
			END IF;
		END;
		ELSE
			INSERT INTO pangya_card(UID, card_typeid, card_type, QNTD, GET_DT, USE_YN)
					VALUES(_IDUSER, _ITEMTYPEID, 1, _C_0, now(), 'N');

			SELECT MAX(CARD_ITEMID) INTO _IDITEM FROM pangya_card WHERE UID = _IDUSER;
		END IF;

		RETURN QUERY SELECT _IDITEM AS ITEM_ID, 'unknown'::varchar(8), 0;
	END;
	ELSIF _NUMERO = 125 THEN --#Card NPC
	BEGIN
		IF _IDITEM <= 0 THEN
			SELECT card_itemid INTO _IDITEM FROM pangya_card 
			WHERE UID = _IDUSER AND card_typeid = _ITEMTYPEID;
		END IF;

		IF _IDITEM > 0 THEN
		DECLARE 
			_ATIVO_C CHAR;
			_C0_QNTD INT;
		BEGIN
			
			SELECT USE_YN INTO _ATIVO_C FROM pangya_card WHERE UID = _IDUSER AND card_itemid = _IDITEM;

			IF _ATIVO = 'N' THEN
			BEGIN
				UPDATE pangya_card SET QNTD = QNTD + _C_0 WHERE UID = _IDUSER AND card_itemid = _IDITEM;

				SELECT QNTD INTO _C0_QNTD FROM pangya_card WHERE UID = _IDUSER AND card_itemid = _IDITEM;

				IF _C0_QNTD <= 0 THEN
					UPDATE pangya_card SET USE_YN = 'Y', QNTD = 0 WHERE UID = _IDUSER AND card_itemid = _IDITEM;
				END IF;
			END;
			ELSE
				UPDATE pangya_card SET QNTD = QNTD + _C_0, USE_YN = 'N' WHERE UID = _IDUSER AND card_itemid = _IDITEM;
			END IF;
		END;
		ELSE
			INSERT INTO pangya_card(UID, card_typeid, card_type, QNTD, GET_DT, USE_YN)
					VALUES(_IDUSER, _ITEMTYPEID, 1, _C_0, now(), 'N');

			SELECT MAX(CARD_ITEMID) INTO _IDITEM FROM pangya_card WHERE UID = _IDUSER;
		END IF;

		RETURN QUERY SELECT _IDITEM AS ITEM_ID, 'unknown'::varchar(8), 0;
	END;
	ELSE --#Por outros itens
		IF _IDITEM <= 0 THEN
			SELECT item_id INTO _IDITEM FROM pangya_item_warehouse 
			WHERE UID = _IDUSER AND typeid = _ITEMTYPEID;
		END IF;

		IF _IDITEM <= 0 THEN
			INSERT INTO pangya_item_warehouse(UID, typeid, valid, regdate, Gift_flag, flag, C0, Purchase, ItemType, c1, c2, c3, c4)
					VALUES(_IDUSER, _ITEMTYPEID, 1, now(), _GIFTFLAG, _TYPEFLAG, _C_0, 0, 2, _C_1, _C_2, _C_3, _C_4);

			SELECT MAX(ITEM_ID) INTO _IDITEM FROM pangya_item_warehouse WHERE UID = _IDUSER;
		END IF;

		RETURN QUERY SELECT _IDITEM AS ITEM_ID, 'unknown'::varchar(8), 0;
	END IF;
END;
$$;


ALTER FUNCTION pangya.usp_add_item(_iduser integer, _giftflag integer, _purchase_in integer, _iditem integer, _itemtypeid integer, _typeflag integer, _typeflag_iff integer, _tempo integer, _c_0 integer, _c_1 integer, _c_2 integer, _c_3 integer, _c_4 integer, _x real, _y real, _z real, _r real) OWNER TO postgres;

--
-- TOC entry 847 (class 1255 OID 23545)
-- Name: usp_add_ucc(integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.usp_add_ucc(_iduser integer, _type_id integer, _giftflag integer, _c_0 integer, _c_1 integer, _c_2 integer, _c_3 integer, _c_4 integer, _purchase_in integer, _typeflag integer, _daycount integer) RETURNS TABLE("_ITEM_ID_" bigint, "_UCC_IDX_" character varying, "_SD_SEQ_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_UCC_IDX varchar(8) DEFAULT NULL;
	_SEQU int;
	_ITEMID bigint;
BEGIN
	_SEQU = 0;

    IF _TYPEFLAG = 8 THEN

        _SEQU = 1;

        << loop_ucc_idx >>
        WHILE 1 = 1 LOOP

            SELECT left(upper(md5(random()::text)), 8) INTO _UCC_IDX;

            IF NOT (CASE WHEN EXISTS 
                    (
                        SELECT 1
                        FROM pangya.tu_ucc
                        WHERE 
                            tu_ucc.UID = _IDUSER AND 
                            tu_ucc.TYPEID = _TYPE_ID AND 
                            tu_ucc.UCCIDX = _UCC_IDX
                    ) THEN 1
                    ELSE 0
                    END <> 0) THEN
                -- exit loop
                exit loop_ucc_idx;
            END IF;

        END LOOP loop_ucc_idx;

    END IF;

    _TYPEFLAG = 5;

    INSERT INTO pangya.pangya_item_warehouse(
        UID, 
        typeid, 
        regdate, 
        Gift_flag, 
        flag, 
        C0, 
        C1, 
        C2, 
        C3, 
        C4, 
        valid, 
        Purchase, 
        ClubSet_WorkShop_Flag, 
        ClubSet_WorkShop_C0, 
        ClubSet_WorkShop_C1, 
        ClubSet_WorkShop_C2, 
        ClubSet_WorkShop_C3, 
        ClubSet_WorkShop_C4, 
        Mastery_Pts, 
        Recovery_Pts, 
        Level, 
        Up, 
        Total_Mastery_Pts, 
        Mastery_Gasto)
    VALUES (
        _IDUSER, 
        _TYPE_ID, 
        now(), 
        _GIFTFLAG, 
        _TYPEFLAG, 
        _C_0, 
        _C_1, 
        _C_2, 
        _C_3, 
        _C_4, 
        1, 
        0, 
        0, 
        0, 
        0, 
        0, 
        0, 
        0, 
        0, 
        0, 
        0, 
        0, 
        0, 
        0)
    RETURNING item_id INTO _ITEMID;

    IF _ITEMID <> 0 THEN

        INSERT INTO pangya.tu_ucc(
            UID, 
            TYPEID, 
            UCCIDX, 
            SEQ, 
            UCC_NAME, 
            USE_YN, 
            IN_DATE, 
            ITEM_ID, 
            STATUS, 
            Flag, 
            TRADE)
        VALUES (
                _IDUSER, 
                _TYPE_ID, 
                _UCC_IDX, 
                _SEQU, 
                NULL, 
                'N', 
                now(), 
                _ITEMID, 
                0, 
                0, 
                0);

        RETURN QUERY SELECT _ITEMID AS ITEM_ID, _UCC_IDX AS UCCIDX, _SEQU AS SD_SEQ;
		
    ELSE 
        RETURN QUERY SELECT 0 AS ITEM_ID, NULL::varchar(8) AS UCCIDX, 0 AS SD_SEQ;
    END IF;
END;
$$;


ALTER FUNCTION pangya.usp_add_ucc(_iduser integer, _type_id integer, _giftflag integer, _c_0 integer, _c_1 integer, _c_2 integer, _c_3 integer, _c_4 integer, _purchase_in integer, _typeflag integer, _daycount integer) OWNER TO postgres;

--
-- TOC entry 848 (class 1255 OID 23546)
-- Name: usp_att_counter_item(integer, integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.usp_att_counter_item(_iduser integer, _count_typeid integer, _id_count integer, _qntd integer, _quest_typeid_high integer, _quest_typeid_low integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF _IDUSER > 0 AND _count_typeid > 0 AND _id_count > 0 AND _qntd > 0 THEN
         UPDATE pangya.count_item
            SET 
               count_num_item = Count_Num_Item + _qntd
         WHERE 
            UID = _IDUSER AND 
            Data_Sec = 0 AND 
            Count_ID = _id_count;
    END IF;
END;
$$;


ALTER FUNCTION pangya.usp_att_counter_item(_iduser integer, _count_typeid integer, _id_count integer, _qntd integer, _quest_typeid_high integer, _quest_typeid_low integer) OWNER TO postgres;

--
-- TOC entry 849 (class 1255 OID 23547)
-- Name: usp_att_course_reward_treasure(); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.usp_att_course_reward_treasure() RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	UPDATE pangya.pangya_course_reward_treasure
         SET 
            PANGREWARD = 
               CASE 
                  WHEN (pangya_course_reward_treasure.PANGREWARD < 950) THEN pangya_course_reward_treasure.PANGREWARD + 50
                  ELSE 
                     CASE 
                        WHEN (pangya_course_reward_treasure.PANGREWARD < 1000) THEN 1000
                        ELSE pangya_course_reward_treasure.PANGREWARD
                     END
               END;
END;
$$;


ALTER FUNCTION pangya.usp_att_course_reward_treasure() OWNER TO postgres;

--
-- TOC entry 850 (class 1255 OID 23548)
-- Name: usp_att_course_reward_treasure_one(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.usp_att_course_reward_treasure_one(_course_in integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	UPDATE pangya.pangya_course_reward_treasure
         SET 
            PANGREWARD = 
               CASE 
                  WHEN ((pangya_course_reward_treasure.PANGREWARD - 1) < 700) THEN 700
                  ELSE pangya_course_reward_treasure.PANGREWARD - 1
               END
      WHERE pangya_course_reward_treasure.COURSE = _course_in;
END;
$$;


ALTER FUNCTION pangya.usp_att_course_reward_treasure_one(_course_in integer) OWNER TO postgres;

--
-- TOC entry 851 (class 1255 OID 23549)
-- Name: usp_char_equip_load_s4(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.usp_char_equip_load_s4(_iduser integer) RETURNS TABLE(_item_id_ bigint, _typeid_ integer, "_ITEMID_PARTS_1_" integer, "_ITEMID_PARTS_2_" integer, "_ITEMID_PARTS_3_" integer, "_ITEMID_PARTS_4_" integer, "_ITEMID_PARTS_5_" integer, "_ITEMID_PARTS_6_" integer, "_ITEMID_PARTS_7_" integer, "_ITEMID_PARTS_8_" integer, "_ITEMID_PARTS_9_" integer, "_ITEMID_PARTS_10_" integer, "_ITEMID_PARTS_11_" integer, "_ITEMID_PARTS_12_" integer, "_ITEMID_PARTS_13_" integer, "_ITEMID_PARTS_14_" integer, "_ITEMID_PARTS_15_" integer, "_ITEMID_PARTS_16_" integer, "_ITEMID_PARTS_17_" integer, "_ITEMID_PARTS_18_" integer, "_ITEMID_PARTS_19_" integer, "_ITEMID_PARTS_20_" integer, "_ITEMID_PARTS_21_" integer, "_ITEMID_PARTS_22_" integer, "_ITEMID_PARTS_23_" integer, "_ITEMID_PARTS_24_" integer, _parts_1_ integer, _parts_2_ integer, _parts_3_ integer, _parts_4_ integer, _parts_5_ integer, _parts_6_ integer, _parts_7_ integer, _parts_8_ integer, _parts_9_ integer, _parts_10_ integer, _parts_11_ integer, _parts_12_ integer, _parts_13_ integer, _parts_14_ integer, _parts_15_ integer, _parts_16_ integer, _parts_17_ integer, _parts_18_ integer, _parts_19_ integer, _parts_20_ integer, _parts_21_ integer, _parts_22_ integer, _parts_23_ integer, _parts_24_ integer, _default_hair_ smallint, _default_shirts_ smallint, _gift_flag_ smallint, "_PCL0_" smallint, "_PCL1_" smallint, "_PCL2_" smallint, "_PCL3_" smallint, "_PCL4_" smallint, "_Purchase_" smallint, _auxparts_1_ integer, _auxparts_2_ integer, _auxparts_3_ integer, _auxparts_4_ integer, _auxparts_5_ integer, "_CutIn_1_" integer, "_CutIn_2_" integer, "_CutIn_3_" integer, "_CutIn_4_" integer, "_Mastery_" integer, "_CARD_CHARACTER_1_" integer, "_CARD_CHARACTER_2_" integer, "_CARD_CHARACTER_3_" integer, "_CARD_CHARACTER_4_" integer, "_CARD_CADDIE_1_" integer, "_CARD_CADDIE_2_" integer, "_CARD_CADDIE_3_" integer, "_CARD_CADDIE_4_" integer, "_CARD_NPC_1_" integer, "_CARD_NPC_2_" integer, "_CARD_NPC_3_" integer, "_CARD_NPC_4_" integer)
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN QUERY SELECT 
         X.item_id, 
         X.typeid, 
         Y.ITEMID_PARTS_1, 
         Y.ITEMID_PARTS_2, 
         Y.ITEMID_PARTS_3, 
         Y.ITEMID_PARTS_4, 
         Y.ITEMID_PARTS_5, 
         Y.ITEMID_PARTS_6, 
         Y.ITEMID_PARTS_7, 
         Y.ITEMID_PARTS_8, 
         Y.ITEMID_PARTS_9, 
         Y.ITEMID_PARTS_10, 
         Y.ITEMID_PARTS_11, 
         Y.ITEMID_PARTS_12, 
         Y.ITEMID_PARTS_13, 
         Y.ITEMID_PARTS_14, 
         Y.ITEMID_PARTS_15, 
         Y.ITEMID_PARTS_16, 
         Y.ITEMID_PARTS_17, 
         Y.ITEMID_PARTS_18, 
         Y.ITEMID_PARTS_19, 
         Y.ITEMID_PARTS_20, 
         Y.ITEMID_PARTS_21, 
         Y.ITEMID_PARTS_22, 
         Y.ITEMID_PARTS_23, 
         Y.ITEMID_PARTS_24, 
         X.parts_1, 
         X.parts_2, 
         X.parts_3, 
         X.parts_4, 
         X.parts_5, 
         X.parts_6, 
         X.parts_7, 
         X.parts_8, 
         X.parts_9, 
         X.parts_10, 
         X.parts_11, 
         X.parts_12, 
         X.parts_13, 
         X.parts_14, 
         X.parts_15, 
         X.parts_16, 
         X.parts_17, 
         X.parts_18, 
         X.parts_19, 
         X.parts_20, 
         X.parts_21, 
         X.parts_22, 
         X.parts_23, 
         X.parts_24, 
         X.default_hair, 
         X.default_shirts, 
         X.gift_flag, 
         X.PCL0, 
         X.PCL1, 
         X.PCL2, 
         X.PCL3, 
         X.PCL4, 
         X.Purchase, 
         X.auxparts_1, 
         X.auxparts_2, 
         X.auxparts_3, 
         X.auxparts_4, 
         X.auxparts_5, 
         X.CutIn_1, 
         X.CutIn_2, 
         X.CutIn_3, 
         X.CutIn_4, 
         X.Mastery, 
         Z.CARD_CHARACTER_1, 
         Z.CARD_CHARACTER_2, 
         Z.CARD_CHARACTER_3, 
         Z.CARD_CHARACTER_4, 
         Z.CARD_CADDIE_1, 
         Z.CARD_CADDIE_2, 
         Z.CARD_CADDIE_3, 
         Z.CARD_CADDIE_4, 
         Z.CARD_NPC_1, 
         Z.CARD_NPC_2, 
         Z.CARD_NPC_3, 
         Z.CARD_NPC_4
      FROM 
         (
            SELECT 
               pangya_character_information.item_id, 
               pangya_character_information.typeid, 
               pangya_character_information.parts_1, 
               pangya_character_information.parts_2, 
               pangya_character_information.parts_3, 
               pangya_character_information.parts_4, 
               pangya_character_information.parts_5, 
               pangya_character_information.parts_6, 
               pangya_character_information.parts_7, 
               pangya_character_information.parts_8, 
               pangya_character_information.parts_9, 
               pangya_character_information.parts_10, 
               pangya_character_information.parts_11, 
               pangya_character_information.parts_12, 
               pangya_character_information.parts_13, 
               pangya_character_information.parts_14, 
               pangya_character_information.parts_15, 
               pangya_character_information.parts_16, 
               pangya_character_information.parts_17, 
               pangya_character_information.parts_18, 
               pangya_character_information.parts_19, 
               pangya_character_information.parts_20, 
               pangya_character_information.parts_21, 
               pangya_character_information.parts_22, 
               pangya_character_information.parts_23, 
               pangya_character_information.parts_24, 
               pangya_character_information.auxparts_1, 
               pangya_character_information.auxparts_2, 
               pangya_character_information.auxparts_3, 
               pangya_character_information.auxparts_4, 
               pangya_character_information.auxparts_5, 
               pangya_character_information.default_hair, 
               pangya_character_information.default_shirts, 
               pangya_character_information.gift_flag, 
               pangya_character_information.PCL0, 
               pangya_character_information.PCL1, 
               pangya_character_information.PCL2, 
               pangya_character_information.PCL3, 
               pangya_character_information.PCL4, 
               pangya_character_information.Purchase, 
               pangya_character_information.CutIn_1, 
               pangya_character_information.CutIn_2, 
               pangya_character_information.CutIn_3, 
               pangya_character_information.CutIn_4, 
               pangya_character_information.Mastery
            FROM pangya.pangya_character_information
            WHERE pangya_character_information.UID = _IDUSER
         )  AS X 
            INNER JOIN 
            (
               SELECT 
                  A.item_id, 
                   max(
                     CASE 
                        WHEN B.EQUIP_NUM = 1 THEN B.ITEMID
                        ELSE 0
                     END) AS ITEMID_PARTS_1, 
                   max(
                     CASE 
                        WHEN B.EQUIP_NUM = 2 THEN B.ITEMID
                        ELSE 0
                     END) AS ITEMID_PARTS_2, 
                   max(
                     CASE 
                        WHEN B.EQUIP_NUM = 3 THEN B.ITEMID
                        ELSE 0
                     END) AS ITEMID_PARTS_3, 
                   max(
                     CASE 
                        WHEN B.EQUIP_NUM = 4 THEN B.ITEMID
                        ELSE 0
                     END) AS ITEMID_PARTS_4, 
                   max(
                     CASE 
                        WHEN B.EQUIP_NUM = 5 THEN B.ITEMID
                        ELSE 0
                     END) AS ITEMID_PARTS_5, 
                   max(
                     CASE 
                        WHEN B.EQUIP_NUM = 6 THEN B.ITEMID
                        ELSE 0
                     END) AS ITEMID_PARTS_6, 
                   max(
                     CASE 
                        WHEN B.EQUIP_NUM = 7 THEN B.ITEMID
                        ELSE 0
                     END) AS ITEMID_PARTS_7, 
                   max(
                     CASE 
                        WHEN B.EQUIP_NUM = 8 THEN B.ITEMID
                        ELSE 0
                     END) AS ITEMID_PARTS_8, 
                   max(
                     CASE 
                        WHEN B.EQUIP_NUM = 9 THEN B.ITEMID
                        ELSE 0
                     END) AS ITEMID_PARTS_9, 
                   max(
                     CASE 
                        WHEN B.EQUIP_NUM = 10 THEN B.ITEMID
                        ELSE 0
                     END) AS ITEMID_PARTS_10, 
                   max(
                     CASE 
                        WHEN B.EQUIP_NUM = 11 THEN B.ITEMID
                        ELSE 0
                     END) AS ITEMID_PARTS_11, 
                   max(
                     CASE 
                        WHEN B.EQUIP_NUM = 12 THEN B.ITEMID
                        ELSE 0
                     END) AS ITEMID_PARTS_12, 
                   max(
                     CASE 
                        WHEN B.EQUIP_NUM = 13 THEN B.ITEMID
                        ELSE 0
                     END) AS ITEMID_PARTS_13, 
                   max(
                     CASE 
                        WHEN B.EQUIP_NUM = 14 THEN B.ITEMID
                        ELSE 0
                     END) AS ITEMID_PARTS_14, 
                   max(
                     CASE 
                        WHEN B.EQUIP_NUM = 15 THEN B.ITEMID
                        ELSE 0
                     END) AS ITEMID_PARTS_15, 
                   max(
                     CASE 
                        WHEN B.EQUIP_NUM = 16 THEN B.ITEMID
                        ELSE 0
                     END) AS ITEMID_PARTS_16, 
                   max(
                     CASE 
                        WHEN B.EQUIP_NUM = 17 THEN B.ITEMID
                        ELSE 0
                     END) AS ITEMID_PARTS_17, 
                   max(
                     CASE 
                        WHEN B.EQUIP_NUM = 18 THEN B.ITEMID
                        ELSE 0
                     END) AS ITEMID_PARTS_18, 
                   max(
                     CASE 
                        WHEN B.EQUIP_NUM = 19 THEN B.ITEMID
                        ELSE 0
                     END) AS ITEMID_PARTS_19, 
                   max(
                     CASE 
                        WHEN B.EQUIP_NUM = 20 THEN B.ITEMID
                        ELSE 0
                     END) AS ITEMID_PARTS_20, 
                   max(
                     CASE 
                        WHEN B.EQUIP_NUM = 21 THEN B.ITEMID
                        ELSE 0
                     END) AS ITEMID_PARTS_21, 
                   max(
                     CASE 
                        WHEN B.EQUIP_NUM = 22 THEN B.ITEMID
                        ELSE 0
                     END) AS ITEMID_PARTS_22, 
                   max(
                     CASE 
                        WHEN B.EQUIP_NUM = 23 THEN B.ITEMID
                        ELSE 0
                     END) AS ITEMID_PARTS_23, 
                   max(
                     CASE 
                        WHEN B.EQUIP_NUM = 24 THEN B.ITEMID
                        ELSE 0
                     END) AS ITEMID_PARTS_24
               FROM 
                  (
                     SELECT pangya_character_information.item_id
                     FROM pangya.pangya_character_information
                     WHERE pangya_character_information.UID = _IDUSER
                  )  AS A 
                     LEFT OUTER JOIN 
                     (
                        SELECT td_char_equip_s4.CHAR_ITEMID, td_char_equip_s4.ITEMID, td_char_equip_s4.EQUIP_NUM
                        FROM pangya.td_char_equip_s4
                        WHERE td_char_equip_s4.UID = _IDUSER AND td_char_equip_s4.USE_YN = 'Y'
                     )  AS B 
                     ON A.item_id = B.CHAR_ITEMID
               GROUP BY A.item_id
            )  AS Y 
			ON X.item_id = Y.item_id
            INNER JOIN 
            (
               SELECT 
                  E.item_id, 
                   max(
                     CASE 
                        WHEN F.Tipo = 0 AND F.Slot = 1 THEN F.card_typeid
                        ELSE 0
                     END) AS CARD_CHARACTER_1, 
                   max(
                     CASE 
                        WHEN F.Tipo = 0 AND F.Slot = 2 THEN F.card_typeid
                        ELSE 0
                     END) AS CARD_CHARACTER_2, 
                   max(
                     CASE 
                        WHEN F.Tipo = 0 AND F.Slot = 3 THEN F.card_typeid
                        ELSE 0
                     END) AS CARD_CHARACTER_3, 
                   max(
                     CASE 
                        WHEN F.Tipo = 0 AND F.Slot = 4 THEN F.card_typeid
                        ELSE 0
                     END) AS CARD_CHARACTER_4, 
                   max(
                     CASE 
                        WHEN F.Tipo = 1 AND F.Slot = 5 THEN F.card_typeid
                        ELSE 0
                     END) AS CARD_CADDIE_1, 
                   max(
                     CASE 
                        WHEN F.Tipo = 1 AND F.Slot = 6 THEN F.card_typeid
                        ELSE 0
                     END) AS CARD_CADDIE_2, 
                   max(
                     CASE 
                        WHEN F.Tipo = 1 AND F.Slot = 7 THEN F.card_typeid
                        ELSE 0
                     END) AS CARD_CADDIE_3, 
                   max(
                     CASE 
                        WHEN F.Tipo = 1 AND F.Slot = 8 THEN F.card_typeid
                        ELSE 0
                     END) AS CARD_CADDIE_4, 
                   max(
                     CASE 
                        WHEN F.Tipo = 5 AND F.Slot = 9 THEN F.card_typeid
                        ELSE 0
                     END) AS CARD_NPC_1, 
                   max(
                     CASE 
                        WHEN F.Tipo = 5 AND F.Slot = 10 THEN F.card_typeid
                        ELSE 0
                     END) AS CARD_NPC_2, 
                   max(
                     CASE 
                        WHEN F.Tipo = 5 AND F.Slot = 11 THEN F.card_typeid
                        ELSE 0
                     END) AS CARD_NPC_3, 
                   max(
                     CASE 
                        WHEN F.Tipo = 5 AND F.Slot = 12 THEN F.card_typeid
                        ELSE 0
                     END) AS CARD_NPC_4
               FROM 
                  (
                     SELECT pangya_character_information.item_id
                     FROM pangya.pangya_character_information
                     WHERE pangya_character_information.UID = _IDUSER
                  )  AS E 
                     LEFT OUTER JOIN 
                     (
                        SELECT pangya_card_equip.Tipo, pangya_card_equip.Slot, pangya_card_equip.card_typeid, pangya_card_equip.parts_id
                        FROM pangya.pangya_card_equip
                        WHERE pangya_card_equip.UID = _IDUSER AND pangya_card_equip.USE_YN = 1
                     )  AS F 
                     ON F.parts_id = E.item_id
               GROUP BY E.item_id               


            )  AS Z 
            ON X.item_id = Z.item_id;
END;
$$;


ALTER FUNCTION pangya.usp_char_equip_load_s4(_iduser integer) OWNER TO postgres;

--
-- TOC entry 852 (class 1255 OID 23551)
-- Name: usp_char_equip_load_s4_one(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.usp_char_equip_load_s4_one(_iduser integer, _iditem integer) RETURNS TABLE(_item_id_ bigint, _typeid_ integer, "_ITEMID_PARTS_1_" integer, "_ITEMID_PARTS_2_" integer, "_ITEMID_PARTS_3_" integer, "_ITEMID_PARTS_4_" integer, "_ITEMID_PARTS_5_" integer, "_ITEMID_PARTS_6_" integer, "_ITEMID_PARTS_7_" integer, "_ITEMID_PARTS_8_" integer, "_ITEMID_PARTS_9_" integer, "_ITEMID_PARTS_10_" integer, "_ITEMID_PARTS_11_" integer, "_ITEMID_PARTS_12_" integer, "_ITEMID_PARTS_13_" integer, "_ITEMID_PARTS_14_" integer, "_ITEMID_PARTS_15_" integer, "_ITEMID_PARTS_16_" integer, "_ITEMID_PARTS_17_" integer, "_ITEMID_PARTS_18_" integer, "_ITEMID_PARTS_19_" integer, "_ITEMID_PARTS_20_" integer, "_ITEMID_PARTS_21_" integer, "_ITEMID_PARTS_22_" integer, "_ITEMID_PARTS_23_" integer, "_ITEMID_PARTS_24_" integer, _parts_1_ integer, _parts_2_ integer, _parts_3_ integer, _parts_4_ integer, _parts_5_ integer, _parts_6_ integer, _parts_7_ integer, _parts_8_ integer, _parts_9_ integer, _parts_10_ integer, _parts_11_ integer, _parts_12_ integer, _parts_13_ integer, _parts_14_ integer, _parts_15_ integer, _parts_16_ integer, _parts_17_ integer, _parts_18_ integer, _parts_19_ integer, _parts_20_ integer, _parts_21_ integer, _parts_22_ integer, _parts_23_ integer, _parts_24_ integer, _default_hair_ smallint, _default_shirts_ smallint, _gift_flag_ smallint, "_PCL0_" smallint, "_PCL1_" smallint, "_PCL2_" smallint, "_PCL3_" smallint, "_PCL4_" smallint, "_Purchase_" smallint, _auxparts_1_ integer, _auxparts_2_ integer, _auxparts_3_ integer, _auxparts_4_ integer, _auxparts_5_ integer, "_CutIn_1_" integer, "_CutIn_2_" integer, "_CutIn_3_" integer, "_CutIn_4_" integer, "_Mastery_" integer, "_CARD_CHARACTER_1_" integer, "_CARD_CHARACTER_2_" integer, "_CARD_CHARACTER_3_" integer, "_CARD_CHARACTER_4_" integer, "_CARD_CADDIE_1_" integer, "_CARD_CADDIE_2_" integer, "_CARD_CADDIE_3_" integer, "_CARD_CADDIE_4_" integer, "_CARD_NPC_1_" integer, "_CARD_NPC_2_" integer, "_CARD_NPC_3_" integer, "_CARD_NPC_4_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_CONTADOR bigint = 0;
BEGIN
	IF _IDITEM < 0 THEN
         SELECT a.character_id INTO _IDITEM
         FROM pangya.pangya_user_equip a
         WHERE a.UID = _IDUSER;
	END IF;
	
	SELECT count(*) INTO _CONTADOR
	FROM pangya.pangya_character_information
	WHERE pangya_character_information.UID = _IDUSER AND pangya_character_information.item_id = _IDITEM;
	
	IF _CONTADOR > 0 THEN

		RETURN QUERY SELECT 
            X.item_id, 
            X.typeid, 
            Y.ITEMID_PARTS_1, 
            Y.ITEMID_PARTS_2, 
            Y.ITEMID_PARTS_3, 
            Y.ITEMID_PARTS_4, 
            Y.ITEMID_PARTS_5, 
            Y.ITEMID_PARTS_6, 
            Y.ITEMID_PARTS_7, 
            Y.ITEMID_PARTS_8, 
            Y.ITEMID_PARTS_9, 
            Y.ITEMID_PARTS_10, 
            Y.ITEMID_PARTS_11, 
            Y.ITEMID_PARTS_12, 
            Y.ITEMID_PARTS_13, 
            Y.ITEMID_PARTS_14, 
            Y.ITEMID_PARTS_15, 
            Y.ITEMID_PARTS_16, 
            Y.ITEMID_PARTS_17, 
            Y.ITEMID_PARTS_18, 
            Y.ITEMID_PARTS_19, 
            Y.ITEMID_PARTS_20, 
            Y.ITEMID_PARTS_21, 
            Y.ITEMID_PARTS_22, 
            Y.ITEMID_PARTS_23, 
            Y.ITEMID_PARTS_24, 
            X.parts_1, 
            X.parts_2, 
            X.parts_3, 
            X.parts_4, 
            X.parts_5, 
            X.parts_6, 
            X.parts_7, 
            X.parts_8, 
            X.parts_9, 
            X.parts_10, 
            X.parts_11, 
            X.parts_12, 
            X.parts_13, 
            X.parts_14, 
            X.parts_15, 
            X.parts_16, 
            X.parts_17, 
            X.parts_18, 
            X.parts_19, 
            X.parts_20, 
            X.parts_21, 
            X.parts_22, 
            X.parts_23, 
            X.parts_24, 
            X.default_hair, 
            X.default_shirts, 
            X.gift_flag, 
            X.PCL0, 
            X.PCL1, 
            X.PCL2, 
            X.PCL3, 
            X.PCL4, 
            X.Purchase, 
            X.auxparts_1, 
            X.auxparts_2, 
            X.auxparts_3, 
            X.auxparts_4, 
            X.auxparts_5, 
            X.CutIn_1,
            X.CutIn_2,
            X.CutIn_3,
            X.CutIn_4,
            X.Mastery, 
            Z.CARD_CHARACTER_1, 
            Z.CARD_CHARACTER_2, 
            Z.CARD_CHARACTER_3, 
            Z.CARD_CHARACTER_4, 
            Z.CARD_CADDIE_1, 
            Z.CARD_CADDIE_2, 
            Z.CARD_CADDIE_3, 
            Z.CARD_CADDIE_4, 
            Z.CARD_NPC_1, 
            Z.CARD_NPC_2, 
            Z.CARD_NPC_3, 
            Z.CARD_NPC_4
         FROM 
            (
               SELECT 
                  pangya_character_information.item_id, 
                  pangya_character_information.typeid, 
                  pangya_character_information.parts_1, 
                  pangya_character_information.parts_2, 
                  pangya_character_information.parts_3, 
                  pangya_character_information.parts_4, 
                  pangya_character_information.parts_5, 
                  pangya_character_information.parts_6, 
                  pangya_character_information.parts_7, 
                  pangya_character_information.parts_8, 
                  pangya_character_information.parts_9, 
                  pangya_character_information.parts_10, 
                  pangya_character_information.parts_11, 
                  pangya_character_information.parts_12, 
                  pangya_character_information.parts_13, 
                  pangya_character_information.parts_14, 
                  pangya_character_information.parts_15, 
                  pangya_character_information.parts_16, 
                  pangya_character_information.parts_17, 
                  pangya_character_information.parts_18, 
                  pangya_character_information.parts_19, 
                  pangya_character_information.parts_20, 
                  pangya_character_information.parts_21, 
                  pangya_character_information.parts_22, 
                  pangya_character_information.parts_23, 
                  pangya_character_information.parts_24, 
                  pangya_character_information.auxparts_1, 
                  pangya_character_information.auxparts_2, 
                  pangya_character_information.auxparts_3, 
                  pangya_character_information.auxparts_4, 
                  pangya_character_information.auxparts_5, 
                  pangya_character_information.default_hair, 
                  pangya_character_information.default_shirts, 
                  pangya_character_information.gift_flag, 
                  pangya_character_information.PCL0, 
                  pangya_character_information.PCL1, 
                  pangya_character_information.PCL2, 
                  pangya_character_information.PCL3, 
                  pangya_character_information.PCL4, 
                  pangya_character_information.Purchase, 
                  pangya_character_information.CutIn_1, 
                  pangya_character_information.CutIn_2, 
                  pangya_character_information.CutIn_3, 
                  pangya_character_information.CutIn_4, 
                  pangya_character_information.Mastery
               FROM pangya.pangya_character_information
               WHERE pangya_character_information.UID = _IDUSER
            )  AS X 
               INNER JOIN 
               (
                  SELECT 
                     A.item_id, 
                      max(
                        CASE 
                           WHEN B.EQUIP_NUM = 1 THEN B.ITEMID
                           ELSE 0
                        END) AS ITEMID_PARTS_1, 
                      max(
                        CASE 
                           WHEN B.EQUIP_NUM = 2 THEN B.ITEMID
                           ELSE 0
                        END) AS ITEMID_PARTS_2, 
                      max(
                        CASE 
                           WHEN B.EQUIP_NUM = 3 THEN B.ITEMID
                           ELSE 0
                        END) AS ITEMID_PARTS_3, 
                      max(
                        CASE 
                           WHEN B.EQUIP_NUM = 4 THEN B.ITEMID
                           ELSE 0
                        END) AS ITEMID_PARTS_4, 
                      max(
                        CASE 
                           WHEN B.EQUIP_NUM = 5 THEN B.ITEMID
                           ELSE 0
                        END) AS ITEMID_PARTS_5, 
                      max(
                        CASE 
                           WHEN B.EQUIP_NUM = 6 THEN B.ITEMID
                           ELSE 0
                        END) AS ITEMID_PARTS_6, 
                      max(
                        CASE 
                           WHEN B.EQUIP_NUM = 7 THEN B.ITEMID
                           ELSE 0
                        END) AS ITEMID_PARTS_7, 
                     max(
                        CASE 
                           WHEN B.EQUIP_NUM = 8 THEN B.ITEMID
                           ELSE 0
                        END) AS ITEMID_PARTS_8, 
                     max(
                        CASE 
                           WHEN B.EQUIP_NUM = 9 THEN B.ITEMID
                           ELSE 0
                        END) AS ITEMID_PARTS_9, 
                     max(
                        CASE 
                           WHEN B.EQUIP_NUM = 10 THEN B.ITEMID
                           ELSE 0
                        END) AS ITEMID_PARTS_10, 
                     max(
                        CASE 
                           WHEN B.EQUIP_NUM = 11 THEN B.ITEMID
                           ELSE 0
                        END) AS ITEMID_PARTS_11, 
                     max(
                        CASE 
                           WHEN B.EQUIP_NUM = 12 THEN B.ITEMID
                           ELSE 0
                        END) AS ITEMID_PARTS_12, 
                     max(
                        CASE 
                           WHEN B.EQUIP_NUM = 13 THEN B.ITEMID
                           ELSE 0
                        END) AS ITEMID_PARTS_13, 
                     max(
                        CASE 
                           WHEN B.EQUIP_NUM = 14 THEN B.ITEMID
                           ELSE 0
                        END) AS ITEMID_PARTS_14, 
                     max(
                        CASE 
                           WHEN B.EQUIP_NUM = 15 THEN B.ITEMID
                           ELSE 0
                        END) AS ITEMID_PARTS_15, 
                     max(
                        CASE 
                           WHEN B.EQUIP_NUM = 16 THEN B.ITEMID
                           ELSE 0
                        END) AS ITEMID_PARTS_16, 
                     max(
                        CASE 
                           WHEN B.EQUIP_NUM = 17 THEN B.ITEMID
                           ELSE 0
                        END) AS ITEMID_PARTS_17, 
                     max(
                        CASE 
                           WHEN B.EQUIP_NUM = 18 THEN B.ITEMID
                           ELSE 0
                        END) AS ITEMID_PARTS_18, 
                     max(
                        CASE 
                           WHEN B.EQUIP_NUM = 19 THEN B.ITEMID
                           ELSE 0
                        END) AS ITEMID_PARTS_19, 
                     max(
                        CASE 
                           WHEN B.EQUIP_NUM = 20 THEN B.ITEMID
                           ELSE 0
                        END) AS ITEMID_PARTS_20, 
                     max(
                        CASE 
                           WHEN B.EQUIP_NUM = 21 THEN B.ITEMID
                           ELSE 0
                        END) AS ITEMID_PARTS_21, 
                     max(
                        CASE 
                           WHEN B.EQUIP_NUM = 22 THEN B.ITEMID
                           ELSE 0
                        END) AS ITEMID_PARTS_22, 
                     max(
                        CASE 
                           WHEN B.EQUIP_NUM = 23 THEN B.ITEMID
                           ELSE 0
                        END) AS ITEMID_PARTS_23, 
                     max(
                        CASE 
                           WHEN B.EQUIP_NUM = 24 THEN B.ITEMID
                           ELSE 0
                        END) AS ITEMID_PARTS_24
                  FROM 
                     (
                        SELECT pangya_character_information.item_id
                        FROM pangya.pangya_character_information
                        WHERE pangya_character_information.UID = _IDUSER
                     )  AS A 
                        LEFT OUTER JOIN 
                        (
                           SELECT td_char_equip_s4.CHAR_ITEMID, td_char_equip_s4.ITEMID, td_char_equip_s4.EQUIP_NUM
                           FROM pangya.td_char_equip_s4
                           WHERE td_char_equip_s4.UID = _IDUSER AND td_char_equip_s4.USE_YN = 'Y'
                        )  AS B 
                        ON A.item_id = B.CHAR_ITEMID
                  GROUP BY A.item_id
               )  AS Y 
			   ON X.item_id = Y.item_id
               INNER JOIN 
               (

                  SELECT 
                     E.item_id, 
                     max(
                        CASE 
                           WHEN F.Tipo = 0 AND F.Slot = 1 THEN F.card_typeid
                           ELSE 0
                        END) AS CARD_CHARACTER_1, 
                     max(
                        CASE 
                           WHEN F.Tipo = 0 AND F.Slot = 2 THEN F.card_typeid
                           ELSE 0
                        END) AS CARD_CHARACTER_2, 
                     max(
                        CASE 
                           WHEN F.Tipo = 0 AND F.Slot = 3 THEN F.card_typeid
                           ELSE 0
                        END) AS CARD_CHARACTER_3, 
                     max(
                        CASE 
                           WHEN F.Tipo = 0 AND F.Slot = 4 THEN F.card_typeid
                           ELSE 0
                        END) AS CARD_CHARACTER_4, 
                     max(
                        CASE 
                           WHEN F.Tipo = 1 AND F.Slot = 5 THEN F.card_typeid
                           ELSE 0
                        END) AS CARD_CADDIE_1, 
                     max(
                        CASE 
                           WHEN F.Tipo = 1 AND F.Slot = 6 THEN F.card_typeid
                           ELSE 0
                        END) AS CARD_CADDIE_2, 
                     max(
                        CASE 
                           WHEN F.Tipo = 1 AND F.Slot = 7 THEN F.card_typeid
                           ELSE 0
                        END) AS CARD_CADDIE_3, 
                     max(
                        CASE 
                           WHEN F.Tipo = 1 AND F.Slot = 8 THEN F.card_typeid
                           ELSE 0
                        END) AS CARD_CADDIE_4, 
                     max(
                        CASE 
                           WHEN F.Tipo = 5 AND F.Slot = 9 THEN F.card_typeid
                           ELSE 0
                        END) AS CARD_NPC_1, 
                     max(
                        CASE 
                           WHEN F.Tipo = 5 AND F.Slot = 10 THEN F.card_typeid
                           ELSE 0
                        END) AS CARD_NPC_2, 
                     max(
                        CASE 
                           WHEN F.Tipo = 5 AND F.Slot = 11 THEN F.card_typeid
                           ELSE 0
                        END) AS CARD_NPC_3, 
                     max(
                        CASE 
                           WHEN F.Tipo = 5 AND F.Slot = 12 THEN F.card_typeid
                           ELSE 0
                        END) AS CARD_NPC_4
                  FROM 
                     (
                        SELECT pangya_character_information.item_id
                        FROM pangya.pangya_character_information
                        WHERE pangya_character_information.UID = _IDUSER
                     )  AS E 
                        LEFT OUTER JOIN 
                        (
                           SELECT pangya_card_equip.Tipo, pangya_card_equip.Slot, pangya_card_equip.card_typeid, pangya_card_equip.parts_id
                           FROM pangya.pangya_card_equip
                           WHERE pangya_card_equip.UID = _IDUSER AND pangya_card_equip.USE_YN = 1
                        )  AS F 
                        ON F.parts_id = E.item_id
                  GROUP BY E.item_id                  


               )  AS Z 
               ON 
                  X.item_id = Z.item_id AND 
                  X.item_id = _IDITEM;
      ELSE 

         RETURN QUERY SELECT 
            0::bigint AS ITEM_ID, 
            0::int AS TYPEID, 
            0::int AS ITEMID_PARTS_1, 
            0::int AS ITEMID_PARTS_2, 
            0::int AS ITEMID_PARTS_3, 
            0::int AS ITEMID_PARTS_4, 
            0::int AS ITEMID_PARTS_5, 
            0::int AS ITEMID_PARTS_6, 
            0::int AS ITEMID_PARTS_7, 
            0::int AS ITEMID_PARTS_8, 
            0::int AS ITEMID_PARTS_9, 
            0::int AS ITEMID_PARTS_10, 
            0::int AS ITEMID_PARTS_11, 
            0::int AS ITEMID_PARTS_12, 
            0::int AS ITEMID_PARTS_13, 
            0::int AS ITEMID_PARTS_14, 
            0::int AS ITEMID_PARTS_15, 
            0::int AS ITEMID_PARTS_16, 
            0::int AS ITEMID_PARTS_17, 
            0::int AS ITEMID_PARTS_18, 
            0::int AS ITEMID_PARTS_19, 
            0::int AS ITEMID_PARTS_20, 
            0::int AS ITEMID_PARTS_21, 
            0::int AS ITEMID_PARTS_22, 
            0::int AS ITEMID_PARTS_23, 
            0::int AS ITEMID_PARTS_24, 
            0::int AS PARTS_1, 
            0::int AS PARTS_2, 
            0::int AS PARTS_3, 
            0::int AS PARTS_4, 
            0::int AS PARTS_5, 
            0::int AS PARTS_6, 
            0::int AS PARTS_7, 
            0::int AS PARTS_8, 
            0::int AS PARTS_9, 
            0::int AS PARTS_10, 
            0::int AS PARTS_11, 
            0::int AS PARTS_12, 
            0::int AS PARTS_13, 
            0::int AS PARTS_14, 
            0::int AS PARTS_15, 
            0::int AS PARTS_16, 
            0::int AS PARTS_17, 
            0::int AS PARTS_18, 
            0::int AS PARTS_19, 
            0::int AS PARTS_20, 
            0::int AS PARTS_21, 
            0::int AS PARTS_22, 
            0::int AS PARTS_23, 
            0::int AS PARTS_24, 
            0::smallint AS DEFAULT_HAIR, 
            0::smallint AS DEFAULT_SHIRTS, 
            0::smallint AS GIFT_FLAG, 
            0::smallint AS PCL0, 
            0::smallint AS PCL1, 
            0::smallint AS PCL2, 
            0::smallint AS PCL3, 
            0::smallint AS PCL4, 
            0::smallint AS PURCHASE, 
            0::int AS AUXPARTS_1, 
            0::int AS AUXPARTS_2, 
            0::int AS AUXPARTS_3, 
            0::int AS AUXPARTS_4, 
            0::int AS AUXPARTS_5, 
            0::int AS CUTIN_1, 
            0::int AS CUTIN_2, 
            0::int AS CUTIN_3, 
            0::int AS CUTIN_4, 
            0::int AS MASTERY, 
            0::int AS CARD_CHARACTER_1, 
            0::int AS CARD_CHARACTER_2, 
            0::int AS CARD_CHARACTER_3, 
            0::int AS CARD_CHARACTER_4, 
            0::int AS CARD_CADDIE_1, 
            0::int AS CARD_CADDIE_2, 
            0::int AS CARD_CADDIE_3, 
            0::int AS CARD_CADDIE_4, 
            0::int AS CARD_NPC_1, 
            0::int AS CARD_NPC_2, 
            0::int AS CARD_NPC_3, 
            0::int AS CARD_NPC_4;
	END IF;
END;
$$;


ALTER FUNCTION pangya.usp_char_equip_load_s4_one(_iduser integer, _iditem integer) OWNER TO postgres;

--
-- TOC entry 853 (class 1255 OID 23553)
-- Name: usp_char_equip_save_s4(integer, integer, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.usp_char_equip_save_s4(_iduser integer, _item_id integer, _str text) RETURNS void
    LANGUAGE plpgsql
    AS $$
DECLARE
	_sparts_1 VARCHAR(13);
	_sparts_2 VARCHAR(13);
	_sparts_3 VARCHAR(13);
	_sparts_4 VARCHAR(13);
	_sparts_5 VARCHAR(13);
	_sparts_6 VARCHAR(13);
	_sparts_7 VARCHAR(13);
	_sparts_8 VARCHAR(13);
	_sparts_9 VARCHAR(13);
	_sparts_10 VARCHAR(13);
	_sparts_11 VARCHAR(13);
	_sparts_12 VARCHAR(13);
	_sparts_13 VARCHAR(13);
	_sparts_14 VARCHAR(13);
	_sparts_15 VARCHAR(13);
	_sparts_16 VARCHAR(13);
	_sparts_17 VARCHAR(13);
	_sparts_18 VARCHAR(13);
	_sparts_19 VARCHAR(13);
	_sparts_20 VARCHAR(13);
	_sparts_21 VARCHAR(13);
	_sparts_22 VARCHAR(13);
	_sparts_23 VARCHAR(13);
	_sparts_24 VARCHAR(13);
	_sparts_id_1 VARCHAR(13);
	_sparts_id_2 VARCHAR(13);
	_sparts_id_3 VARCHAR(13);
	_sparts_id_4 VARCHAR(13);
	_sparts_id_5 VARCHAR(13);
	_sparts_id_6 VARCHAR(13);
	_sparts_id_7 VARCHAR(13);
	_sparts_id_8 VARCHAR(13);
	_sparts_id_9 VARCHAR(13);
	_sparts_id_10 VARCHAR(13);
	_sparts_id_11 VARCHAR(13);
	_sparts_id_12 VARCHAR(13);
	_sparts_id_13 VARCHAR(13);
	_sparts_id_14 VARCHAR(13);
	_sparts_id_15 VARCHAR(13);
	_sparts_id_16 VARCHAR(13);
	_sparts_id_17 VARCHAR(13);
	_sparts_id_18 VARCHAR(13);
	_sparts_id_19 VARCHAR(13);
	_sparts_id_20 VARCHAR(13);
	_sparts_id_21 VARCHAR(13);
	_sparts_id_22 VARCHAR(13);
	_sparts_id_23 VARCHAR(13);
	_sparts_id_24 VARCHAR(13);
	_sdefault_hair VARCHAR(13);
	_sdefault_shirts VARCHAR(13);
	_sgift_flag VARCHAR(13);
	_spurchase VARCHAR(13);
	_sPCL_1 VARCHAR(13);
	_sPCL_2 VARCHAR(13);
	_sPCL_3 VARCHAR(13);
	_sPCL_4 VARCHAR(13);
	_sPCL_5 VARCHAR(13);
	_scutin_1 VARCHAR(13);
	_scutin_2 VARCHAR(13);
	_scutin_3 VARCHAR(13);
	_scutin_4 VARCHAR(13);
	_sauxparts_1 VARCHAR(13);
	_sauxparts_2 VARCHAR(13);
	_sauxparts_3 VARCHAR(13);
	_sauxparts_4 VARCHAR(13);
	_sauxparts_5 VARCHAR(13);
	_smastery VARCHAR(13);
BEGIN
	_sdefault_hair = split_part(_STR,'|',2);
    _sdefault_shirts = split_part(_STR,'|',3);
    _sgift_flag = split_part(_STR,'|',4);
    _spurchase = split_part(_STR,'|',5);
	_sparts_1 = split_part(_STR,'|',6);
	_sparts_2 = split_part(_STR,'|',7);
	_sparts_3 = split_part(_STR,'|',8);
	_sparts_4 = split_part(_STR,'|',9);
	_sparts_5 = split_part(_STR,'|',10);
	_sparts_6 = split_part(_STR,'|',11);
	_sparts_7 = split_part(_STR,'|',12);
	_sparts_8 = split_part(_STR,'|',13);
	_sparts_9 = split_part(_STR,'|',14);
	_sparts_10 = split_part(_STR,'|',15);
	_sparts_11 = split_part(_STR,'|',16);
	_sparts_12 = split_part(_STR,'|',17);
	_sparts_13 = split_part(_STR,'|',18);
	_sparts_14 = split_part(_STR,'|',19);
	_sparts_15 = split_part(_STR,'|',20);
	_sparts_16 = split_part(_STR,'|',21);
	_sparts_17 = split_part(_STR,'|',22);
	_sparts_18 = split_part(_STR,'|',23);
	_sparts_19 = split_part(_STR,'|',24);
	_sparts_20 = split_part(_STR,'|',25);
	_sparts_21 = split_part(_STR,'|',26);
	_sparts_22 = split_part(_STR,'|',27);
	_sparts_23 = split_part(_STR,'|',28);
	_sparts_24 = split_part(_STR,'|',29);
	_sparts_id_1 = split_part(_STR,'|',30);
	_sparts_id_2 = split_part(_STR,'|',31);
	_sparts_id_3 = split_part(_STR,'|',32);
	_sparts_id_4 = split_part(_STR,'|',33);
	_sparts_id_5 = split_part(_STR,'|',34);
	_sparts_id_6 = split_part(_STR,'|',35);
	_sparts_id_7 = split_part(_STR,'|',36);
	_sparts_id_8 = split_part(_STR,'|',37);
	_sparts_id_9 = split_part(_STR,'|',38);
	_sparts_id_10 = split_part(_STR,'|',39);
	_sparts_id_11 = split_part(_STR,'|',40);
	_sparts_id_12 = split_part(_STR,'|',41);
	_sparts_id_13 = split_part(_STR,'|',42);
	_sparts_id_14 = split_part(_STR,'|',43);
	_sparts_id_15 = split_part(_STR,'|',44);
	_sparts_id_16 = split_part(_STR,'|',45);
	_sparts_id_17 = split_part(_STR,'|',46);
	_sparts_id_18 = split_part(_STR,'|',47);
	_sparts_id_19 = split_part(_STR,'|',48);
	_sparts_id_20 = split_part(_STR,'|',49);
	_sparts_id_21 = split_part(_STR,'|',50);
	_sparts_id_22 = split_part(_STR,'|',51);
	_sparts_id_23 = split_part(_STR,'|',52);
	_sparts_id_24 = split_part(_STR,'|',53);
	_sauxparts_1 = split_part(_STR,'|',54);
	_sauxparts_2 = split_part(_STR,'|',55);
	_sauxparts_3 = split_part(_STR,'|',56);
	_sauxparts_4 = split_part(_STR,'|',57);
	_sauxparts_5 = split_part(_STR,'|',58);
    _scutin_1 = split_part(_STR,'|',59);
    _scutin_2 = split_part(_STR,'|',60);
    _scutin_3 = split_part(_STR,'|',61);
    _scutin_4 = split_part(_STR,'|',62);
	_sPCL_1 = split_part(_STR,'|',63);
	_sPCL_2 = split_part(_STR,'|',64);
	_sPCL_3 = split_part(_STR,'|',65);
	_sPCL_4 = split_part(_STR,'|',66);
	_sPCL_5 = split_part(_STR,'|',67);
    _smastery = split_part(_STR,'|',68);
	
	UPDATE pangya_character_information 
		SET parts_1 = _sparts_1::int
			,parts_2 = _sparts_2::int
			,parts_3 = _sparts_3::int
			,parts_4 = _sparts_4::int
			,parts_5 = _sparts_5::int
			,parts_6 = _sparts_6::int
			,parts_7 = _sparts_7::int
			,parts_8 = _sparts_8::int
			,parts_9 = _sparts_9::int
			,parts_10 = _sparts_10::int
			,parts_11 = _sparts_11::int
			,parts_12 = _sparts_12::int
			,parts_13 = _sparts_13::int
			,parts_14 = _sparts_14::int
			,parts_15 = _sparts_15::int
			,parts_16 = _sparts_16::int
			,parts_17 = _sparts_17::int
			,parts_18 = _sparts_18::int
			,parts_19 = _sparts_19::int
			,parts_20 = _sparts_20::int
			,parts_21 = _sparts_21::int
			,parts_22 = _sparts_22::int
			,parts_23 = _sparts_23::int
			,parts_24 = _sparts_24::int
			,default_hair = _sdefault_hair::smallint
			,default_shirts = _sdefault_shirts::smallint
			,gift_flag = _sgift_flag::smallint
			,purchase = _spurchase::smallint
			,PCL0 = _sPCL_1::smallint
			,PCL1 = _sPCL_2::smallint
			,PCL2 = _sPCL_3::smallint
			,PCL3 = _sPCL_4::smallint
			,PCL4 = _sPCL_5::smallint
			,auxparts_1 = _sauxparts_1::int
			,auxparts_2 = _sauxparts_2::int
			,auxparts_3 = _sauxparts_3::int
			,auxparts_4 = _sauxparts_4::int
			,auxparts_5 = _sauxparts_5::int
			,cutin_1 = _scutin_1::int
			,cutin_2 = _scutin_2::int
			,cutin_3 = _scutin_3::int
			,cutin_4 = _scutin_4::int
			,mastery = _smastery::int
	WHERE UID = _IDUSER AND item_id = _ITEM_ID;
	
	IF (SELECT CHAR_ITEMID FROM pangya.td_char_equip_s4 WHERE UID = _IDUSER AND CHAR_ITEMID = _ITEM_ID LIMIT 1) IS NULL THEN
	DECLARE 
		_COUNT INT = 0;
	BEGIN

		WHILE _COUNT < 24 LOOP
			INSERT INTO pangya.td_char_equip_s4(UID, CHAR_ITEMID, IN_DATE, EQUIP_NUM, USE_YN)
			VALUES(_IDUSER, _ITEM_ID, now(), _COUNT + 1, 'Y');
			
			_COUNT = _COUNT + 1;
		END LOOP;
	END;
	END IF;
	
	UPDATE td_char_equip_s4 SET ITEMID = _sparts_id_1::int, EQUIP_TYPE = CASE WHEN _sparts_id_1::int > 0 THEN _sparts_1::int ELSE 0 END
	WHERE UID = _IDUSER AND EQUIP_NUM = 1 AND CHAR_ITEMID = _ITEM_ID;
	UPDATE td_char_equip_s4 SET ITEMID = _sparts_id_2::int, EQUIP_TYPE = CASE WHEN _sparts_id_2::int > 0 THEN _sparts_2::int ELSE 0 END
	WHERE UID = _IDUSER AND EQUIP_NUM = 2 AND CHAR_ITEMID = _ITEM_ID;
	UPDATE td_char_equip_s4 SET ITEMID = _sparts_id_3::int, EQUIP_TYPE = CASE WHEN _sparts_id_3::int > 0 THEN _sparts_3::int ELSE 0 END
	WHERE UID = _IDUSER AND EQUIP_NUM = 3 AND CHAR_ITEMID = _ITEM_ID;
	UPDATE td_char_equip_s4 SET ITEMID = _sparts_id_4::int, EQUIP_TYPE = CASE WHEN _sparts_id_4::int > 0 THEN _sparts_4::int ELSE 0 END
	WHERE UID = _IDUSER AND EQUIP_NUM = 4 AND CHAR_ITEMID = _ITEM_ID;
	UPDATE td_char_equip_s4 SET ITEMID = _sparts_id_5::int, EQUIP_TYPE = CASE WHEN _sparts_id_5::int > 0 THEN _sparts_5::int ELSE 0 END
	WHERE UID = _IDUSER AND EQUIP_NUM = 5 AND CHAR_ITEMID = _ITEM_ID;
	UPDATE td_char_equip_s4 SET ITEMID = _sparts_id_6::int, EQUIP_TYPE = CASE WHEN _sparts_id_6::int > 0 THEN _sparts_6::int ELSE 0 END
	WHERE UID = _IDUSER AND EQUIP_NUM = 6 AND CHAR_ITEMID = _ITEM_ID;
	UPDATE td_char_equip_s4 SET ITEMID = _sparts_id_7::int, EQUIP_TYPE = CASE WHEN _sparts_id_7::int > 0 THEN _sparts_7::int ELSE 0 END
	WHERE UID = _IDUSER AND EQUIP_NUM = 7 AND CHAR_ITEMID = _ITEM_ID;
	UPDATE td_char_equip_s4 SET ITEMID = _sparts_id_8::int, EQUIP_TYPE = CASE WHEN _sparts_id_8::int > 0 THEN _sparts_8::int ELSE 0 END
	WHERE UID = _IDUSER AND EQUIP_NUM = 8 AND CHAR_ITEMID = _ITEM_ID;
	UPDATE td_char_equip_s4 SET ITEMID = _sparts_id_9::int, EQUIP_TYPE = CASE WHEN _sparts_id_9::int > 0 THEN _sparts_9::int ELSE 0 END
	WHERE UID = _IDUSER AND EQUIP_NUM = 9 AND CHAR_ITEMID = _ITEM_ID;
	UPDATE td_char_equip_s4 SET ITEMID = _sparts_id_10::int, EQUIP_TYPE = CASE WHEN _sparts_id_10::int > 0 THEN _sparts_10::int ELSE 0 END
	WHERE UID = _IDUSER AND EQUIP_NUM = 10 AND CHAR_ITEMID = _ITEM_ID;
	UPDATE td_char_equip_s4 SET ITEMID = _sparts_id_11::int, EQUIP_TYPE = CASE WHEN _sparts_id_11::int > 0 THEN _sparts_11::int ELSE 0 END
	WHERE UID = _IDUSER AND EQUIP_NUM = 11 AND CHAR_ITEMID = _ITEM_ID;
	UPDATE td_char_equip_s4 SET ITEMID = _sparts_id_12::int, EQUIP_TYPE = CASE WHEN _sparts_id_12::int > 0 THEN _sparts_12::int ELSE 0 END
	WHERE UID = _IDUSER AND EQUIP_NUM = 12 AND CHAR_ITEMID = _ITEM_ID;
	UPDATE td_char_equip_s4 SET ITEMID = _sparts_id_13::int, EQUIP_TYPE = CASE WHEN _sparts_id_13::int > 0 THEN _sparts_13::int ELSE 0 END
	WHERE UID = _IDUSER AND EQUIP_NUM = 13 AND CHAR_ITEMID = _ITEM_ID;
	UPDATE td_char_equip_s4 SET ITEMID = _sparts_id_14::int, EQUIP_TYPE = CASE WHEN _sparts_id_14::int > 0 THEN _sparts_14::int ELSE 0 END
	WHERE UID = _IDUSER AND EQUIP_NUM = 14 AND CHAR_ITEMID = _ITEM_ID;
	UPDATE td_char_equip_s4 SET ITEMID = _sparts_id_15::int, EQUIP_TYPE = CASE WHEN _sparts_id_15::int > 0 THEN _sparts_15::int ELSE 0 END
	WHERE UID = _IDUSER AND EQUIP_NUM = 15 AND CHAR_ITEMID = _ITEM_ID;
	UPDATE td_char_equip_s4 SET ITEMID = _sparts_id_16::int, EQUIP_TYPE = CASE WHEN _sparts_id_16::int > 0 THEN _sparts_16::int ELSE 0 END
	WHERE UID = _IDUSER AND EQUIP_NUM = 16 AND CHAR_ITEMID = _ITEM_ID;
	UPDATE td_char_equip_s4 SET ITEMID = _sparts_id_17::int, EQUIP_TYPE = CASE WHEN _sparts_id_17::int > 0 THEN _sparts_17::int ELSE 0 END
	WHERE UID = _IDUSER AND EQUIP_NUM = 17 AND CHAR_ITEMID = _ITEM_ID;
	UPDATE td_char_equip_s4 SET ITEMID = _sparts_id_18::int, EQUIP_TYPE = CASE WHEN _sparts_id_18::int > 0 THEN _sparts_18::int ELSE 0 END
	WHERE UID = _IDUSER AND EQUIP_NUM = 18 AND CHAR_ITEMID = _ITEM_ID;
	UPDATE td_char_equip_s4 SET ITEMID = _sparts_id_19::int, EQUIP_TYPE = CASE WHEN _sparts_id_19::int > 0 THEN _sparts_19::int ELSE 0 END
	WHERE UID = _IDUSER AND EQUIP_NUM = 19 AND CHAR_ITEMID = _ITEM_ID;
	UPDATE td_char_equip_s4 SET ITEMID = _sparts_id_20::int, EQUIP_TYPE = CASE WHEN _sparts_id_20::int > 0 THEN _sparts_20::int ELSE 0 END
	WHERE UID = _IDUSER AND EQUIP_NUM = 20 AND CHAR_ITEMID = _ITEM_ID;
	UPDATE td_char_equip_s4 SET ITEMID = _sparts_id_21::int, EQUIP_TYPE = CASE WHEN _sparts_id_21::int > 0 THEN _sparts_21::int ELSE 0 END
	WHERE UID = _IDUSER AND EQUIP_NUM = 21 AND CHAR_ITEMID = _ITEM_ID;
	UPDATE td_char_equip_s4 SET ITEMID = _sparts_id_22::int, EQUIP_TYPE = CASE WHEN _sparts_id_22::int > 0 THEN _sparts_22::int ELSE 0 END
	WHERE UID = _IDUSER AND EQUIP_NUM = 22 AND CHAR_ITEMID = _ITEM_ID;
	UPDATE td_char_equip_s4 SET ITEMID = _sparts_id_23::int, EQUIP_TYPE = CASE WHEN _sparts_id_23::int > 0 THEN _sparts_23::int ELSE 0 END
	WHERE UID = _IDUSER AND EQUIP_NUM = 23 AND CHAR_ITEMID = _ITEM_ID;
	UPDATE td_char_equip_s4 SET ITEMID = _sparts_id_24::int, EQUIP_TYPE = CASE WHEN _sparts_id_24::int > 0 THEN _sparts_24::int ELSE 0 END
	WHERE UID = _IDUSER AND EQUIP_NUM = 24 AND CHAR_ITEMID = _ITEM_ID;
END;
$$;


ALTER FUNCTION pangya.usp_char_equip_save_s4(_iduser integer, _item_id integer, _str text) OWNER TO postgres;

--
-- TOC entry 854 (class 1255 OID 23555)
-- Name: usp_char_user_equip(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.usp_char_user_equip(_iduser integer) RETURNS TABLE(_caddie_id_ integer, _character_id_ integer, _club_id_ integer, _ball_type_ integer, _item_slot_1_ integer, _item_slot_2_ integer, _item_slot_3_ integer, _item_slot_4_ integer, _item_slot_5_ integer, _item_slot_6_ integer, _item_slot_7_ integer, _item_slot_8_ integer, _item_slot_9_ integer, _item_slot_10_ integer, _skin_id_1_ bigint, _skin_id_2_ bigint, _skin_id_3_ bigint, _skin_id_4_ bigint, _skin_id_5_ bigint, _skin_id_6_ bigint, "_Skin_1_" integer, "_Skin_2_" integer, "_Skin_3_" integer, "_Skin_4_" integer, "_Skin_5_" integer, "_Skin_6_" integer, _mascot_id_ integer, _poster_1_ integer, _poster_2_ integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_Skin_Temp int = 0;
BEGIN
	RETURN QUERY SELECT 
        caddie_id, 
        character_id, 
        club_id, 
        ball_type, 
        item_slot_1, 
        item_slot_2, 
        item_slot_3, 
        item_slot_4, 
        item_slot_5, 
        item_slot_6, 
        item_slot_7, 
        item_slot_8, 
        item_slot_9, 
        item_slot_10, 
        COALESCE(
        (
            SELECT pangya_item_warehouse.item_id
            FROM pangya.pangya_item_warehouse
            WHERE pangya_item_warehouse.UID = _IDUSER AND pangya_item_warehouse.typeid = pangya_user_equip.Skin_1
            ORDER BY EndDate DESC LIMIT 1
        ), '0') AS skin_id_1, 
        COALESCE(
        (
            SELECT pangya_item_warehouse.item_id
            FROM pangya.pangya_item_warehouse
            WHERE pangya_item_warehouse.UID = _IDUSER AND pangya_item_warehouse.typeid = pangya_user_equip.Skin_2
            ORDER BY EndDate DESC LIMIT 1
        ), '0') AS skin_id_2, 
        COALESCE(
        (
            SELECT pangya_item_warehouse.item_id
            FROM pangya.pangya_item_warehouse
            WHERE pangya_item_warehouse.UID = _IDUSER AND pangya_item_warehouse.typeid = pangya_user_equip.Skin_3
            ORDER BY EndDate DESC LIMIT 1
        ), '0') AS skin_id_3, 
        COALESCE(
        (
            SELECT pangya_item_warehouse.item_id
            FROM pangya.pangya_item_warehouse
            WHERE pangya_item_warehouse.UID = _IDUSER AND pangya_item_warehouse.typeid = pangya_user_equip.Skin_4
            ORDER BY EndDate DESC LIMIT 1
        ), '0') AS skin_id_4, 
        COALESCE(
        (
            SELECT pangya_item_warehouse.item_id
            FROM pangya.pangya_item_warehouse
            WHERE pangya_item_warehouse.UID = _IDUSER AND pangya_item_warehouse.typeid = pangya_user_equip.Skin_5
            ORDER BY EndDate DESC LIMIT 1
        ), '0') AS skin_id_5, 
        COALESCE(
        (
            SELECT pangya_item_warehouse.item_id
            FROM pangya.pangya_item_warehouse
            WHERE pangya_item_warehouse.UID = _IDUSER AND pangya_item_warehouse.typeid = pangya_user_equip.Skin_6
            ORDER BY EndDate DESC LIMIT 1
        ), '0') AS skin_id_6, 
        Skin_1, 
        Skin_2, 
        Skin_3, 
        Skin_4, 
        Skin_5, 
        Skin_6, 
        mascot_id, 
        poster_1, 
        poster_2
    FROM pangya.pangya_user_equip
    WHERE UID = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.usp_char_user_equip(_iduser integer) OWNER TO postgres;

--
-- TOC entry 855 (class 1255 OID 23556)
-- Name: usp_createachievementuser(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.usp_createachievementuser(_iduser integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	INSERT INTO ACHIEVEMENT_TIPO(UID, NOME, TYPEID, TIPO, OPTION) SELECT _IDUSER, nome, typeid, tipo, option FROM achievements group by typeid, nome, typeid, tipo, option, index ORDER BY index;

	--#INSERT INTO COUNT_ITEM(UID, NOME, TYPEID, TIPO) SELECT IDUSER, nome, typeid, 1 as tipo_ FROM counter_items;

	INSERT INTO ACHIEVEMENT_QUEST(UID, ID_ACHIEVEMENT, TYPEID_ACHIEVE, COUNT_ID, OBJETIVO_QUEST) SELECT _IDUSER, x.ID_ACHIEVEMENT, x.typeid_d, 0, x.objetivo_quest FROM
	(
		SELECT e.* FROM
			(SELECT c.*, d.* FROM
				(SELECT a.*, b.* FROM (SELECT * FROM ACHIEVEMENT_TIPO) a
				INNER JOIN
				(SELECT typeid as typeid_b, quest_typeid, index as index_b FROM achievements) b
				ON a.typeid = b.typeid_b) as c
			INNER JOIN
			(SELECT typeid as typeid_d, counter_typeid, counter_qntd as Objetivo_Quest FROM quest_stuffs) d
			ON c.quest_typeid = d.typeid_d) e
            WHERE e.UID = _IDUSER
	)x  ORDER BY x.index_b;
    
    INSERT INTO COUNT_ITEM(UID, NOME, ID_ACHIEVEMENT, TYPEID, TIPO) SELECT _IDUSER, K.NOME, G.ID_ACHIEVEMENT, K.TYPEID, 1 FROM
		(SELECT J.*, H.* FROM
		(SELECT _IDUSER as UID, x.ID_ACHIEVEMENT, x.typeid_d, 0 as column4, x.objetivo_quest FROM
			(
				SELECT e.* FROM
					(SELECT c.*, d.* FROM
						(SELECT a.*, b.* FROM (SELECT * FROM ACHIEVEMENT_TIPO) a
						INNER JOIN
						(SELECT typeid as typeid_b, quest_typeid, index as index_b FROM achievements) b
						ON a.typeid = b.typeid_b) c
					INNER JOIN
					(SELECT typeid as typeid_d, counter_typeid, counter_qntd as Objetivo_Quest FROM quest_stuffs) d
					ON c.quest_typeid = d.typeid_d) e
					WHERE e.UID = _IDUSER
			)x 
			GROUP BY x.ID_ACHIEVEMENT, x.typeid_d, x.Objetivo_Quest, x.index_b ORDER BY x.index_b
			LIMIT 9223372036854775807) J
		INNER JOIN
        (SELECT * FROM quest_stuffs) H
        ON J.typeid_d = H.typeid) G
        INNER JOIN
        (SELECT * FROM counter_items) K
	ON K.typeid = G.counter_typeid;
    
    UPDATE a
		SET a.count_id = b.count_id
	FROM pangya.achievement_quest a
	INNER JOIN (SELECT * FROM pangya.count_item) b
	ON a.uid = _IDUSER AND a.ID_ACHIEVEMENT = b.ID_ACHIEVEMENT;
END;
$$;


ALTER FUNCTION pangya.usp_createachievementuser(_iduser integer) OWNER TO postgres;

--
-- TOC entry 856 (class 1255 OID 23557)
-- Name: usp_exp_caddie_att(integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.usp_exp_caddie_att(_iduser integer, _caddie_id integer, _xp integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
DECLARE 
	_cEXP_LVL INT default 0;
	_cLvl INT;
	_exp INT;
BEGIN
	SELECT cLevel INTO _cLvl FROM pangya_caddie_information WHERE UID = _IDUSER AND item_id = _caddie_id;
    
    IF _cLvl < 3 THEN
		SELECT Exp INTO _exp FROM pangya_caddie_information WHERE UID = _IDUSER AND item_id = _caddie_id;
        
        _exp = _exp + _xp;
		
        << loop_add_exp >>
		WHILE 1 = 1 LOOP

			_cEXP_LVL = 520 + (160 * _cLvl);
			
			IF _exp >= _cEXP_LVL THEN
				
                _cLvl = _cLvl + 1;
                
                IF _cLvl = 3 THEN
					UPDATE pangya_caddie_information SET Exp = 0, cLevel = _cLvl
					WHERE UID = _IDUSER AND item_id = _caddie_id;

                    -- exit loop
                    exit loop_add_exp;
                ELSE
					_exp = _exp - _cEXP_LVL;
					
					UPDATE pangya_caddie_information SET Exp = _exp, cLevel = _cLvl
					WHERE UID = _IDUSER AND item_id = _caddie_id;
                END IF;
			ELSE
				UPDATE pangya_caddie_information SET Exp = _exp WHERE UID = _IDUSER AND item_id = _caddie_id;
                
                -- exit loop
                exit loop_add_exp;
			END IF;

		END LOOP loop_add_exp;
	END IF;
END;
$$;


ALTER FUNCTION pangya.usp_exp_caddie_att(_iduser integer, _caddie_id integer, _xp integer) OWNER TO postgres;

--
-- TOC entry 857 (class 1255 OID 23558)
-- Name: usp_exp_mascot_att(integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.usp_exp_mascot_att(_iduser integer, _mascot_id integer, _xp integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
DECLARE 
	_mEXP_LVL int default 0;
	_mLvl INT;
	_exp INT;
BEGIN
	SELECT mLevel INTO _mLvl FROM pangya_mascot_info WHERE UID = _IDUSER AND item_id = _mascot_id;
    
    IF _mLvl < 9 THEN

		SELECT mExp INTO _exp FROM pangya_mascot_info WHERE UID = _IDUSER AND item_id = _mascot_id;
        
        _exp = _exp + _xp;
		
        << loop_add_exp >>
		WHILE 1 = 1 LOOP

			_mEXP_LVL = 50 + (10 + (10 * _mLvl));
			
			IF _exp >= _mEXP_LVL THEN

				_mLvl = _mLvl + 1;
                
                IF _mLvl = 9 THEN
					UPDATE pangya_mascot_info SET mExp = 0, mLevel = _mLvl
					WHERE UID = _IDUSER AND item_id = _mascot_id;
                    
                    -- exit loop
                    exit loop_add_exp;
                ELSE
					_exp = _exp - _mEXP_LVL;
					
					UPDATE pangya_mascot_info SET mExp = _exp, mLevel = _mLvl
					WHERE UID = _IDUSER AND item_id = _mascot_id;
                END IF;
			ELSE
				UPDATE pangya_mascot_info SET mExp = _exp WHERE UID = _IDUSER AND item_id = _mascot_id;
                
                -- exit loop
                exit loop_add_exp;
			END IF;

		END LOOP loop_add_exp;
	END IF;
END;
$$;


ALTER FUNCTION pangya.usp_exp_mascot_att(_iduser integer, _mascot_id integer, _xp integer) OWNER TO postgres;

--
-- TOC entry 859 (class 1255 OID 23559)
-- Name: usp_exp_player_att(integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.usp_exp_player_att(_iduser integer, _exp integer, _opt_in integer) RETURNS TABLE("_OPT_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_OPT INT default 0;
	_caddie_id INT;
	_mascot_id INT;
	_EXP_1 INT;
	_EXP_2 INT;
BEGIN
	IF _OPT_IN = 0 THEN --# add o EXP para o caddie e mascot tbm se for 0 a opção
		--#ATT EXP DO CADDIE TBM
		SELECT caddie_id INTO _caddie_id FROM pangya_user_equip WHERE UID = _IDUSER;
		
		IF _caddie_id != 0 AND _caddie_id IS NOT NULL THEN
			perform pangya.USP_EXP_CADDIE_ATT(_IDUSER, _caddie_id, _EXP);
		END IF;
        
        SELECT mascot_id INTO _mascot_id FROM pangya_user_equip WHERE UID = _IDUSER;
        
        IF _mascot_id != 0 AND _mascot_id IS NOT NULL THEN
			perform pangya.USP_EXP_MASCOT_ATT(_IDUSER, _mascot_id, _EXP);
        END IF;
    END IF;

	SELECT xp INTO _EXP_1 FROM user_info WHERE UID = _IDUSER;
    
    _EXP_1 = _EXP_1 + _EXP;

    << loop_add_exp >>
	WHILE 1 = 1 LOOP

        SELECT pangya.GetEXPByLevel(_IDUSER) INTO _EXP_2;

		IF _EXP_1 >= _EXP_2 THEN
		BEGIN
			_EXP_1 = _EXP_1 - _EXP_2;

			UPDATE user_info SET level = level + 1, xp = _EXP_1
			WHERE UID = _IDUSER;

			perform USP_LEVEL_REWARD(_IDUSER);

			_OPT = 1;
		END;
		ELSE
			UPDATE user_info SET xp = _EXP_1 WHERE UID = _IDUSER;

            -- exit loop
			exit loop_add_exp;
		END IF;

	END LOOP loop_add_exp;

	RETURN QUERY SELECT _OPT AS OPT;
END;
$$;


ALTER FUNCTION pangya.usp_exp_player_att(_iduser integer, _exp integer, _opt_in integer) OWNER TO postgres;

--
-- TOC entry 860 (class 1255 OID 23560)
-- Name: usp_flush_caddie(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.usp_flush_caddie(_iduser integer, _idcaddie integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	UPDATE pangya.pangya_user_equip
         SET 
            caddie_id = _IDCADDIE
      WHERE pangya_user_equip.UID = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.usp_flush_caddie(_iduser integer, _idcaddie integer) OWNER TO postgres;

--
-- TOC entry 861 (class 1255 OID 23561)
-- Name: usp_flush_character(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.usp_flush_character(_iduser integer, _idcharacter integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	UPDATE pangya.pangya_user_equip
         SET 
            character_id = _IDCHARACTER
      WHERE pangya_user_equip.UID = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.usp_flush_character(_iduser integer, _idcharacter integer) OWNER TO postgres;

--
-- TOC entry 862 (class 1255 OID 23562)
-- Name: usp_flush_club(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.usp_flush_club(_iduser integer, _idclub integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	UPDATE pangya.pangya_user_equip
         SET 
            club_id = _IDCLUB
      WHERE pangya_user_equip.UID = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.usp_flush_club(_iduser integer, _idclub integer) OWNER TO postgres;

--
-- TOC entry 863 (class 1255 OID 23563)
-- Name: usp_flush_comet(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.usp_flush_comet(_iduser integer, _idcomet integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	UPDATE pangya.pangya_user_equip
         SET 
            ball_type = _IDCOMET
      WHERE pangya_user_equip.UID = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.usp_flush_comet(_iduser integer, _idcomet integer) OWNER TO postgres;

--
-- TOC entry 864 (class 1255 OID 23564)
-- Name: usp_flush_equip_character(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.usp_flush_equip_character(_iduser integer, _idcharacter integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	UPDATE pangya.pangya_user_equip
         SET 
            character_id = _IDCHARACTER
      WHERE pangya_user_equip.UID = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.usp_flush_equip_character(_iduser integer, _idcharacter integer) OWNER TO postgres;

--
-- TOC entry 865 (class 1255 OID 23565)
-- Name: usp_flush_equip_comet_clubset(integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.usp_flush_equip_comet_clubset(_iduser integer, _idcomet integer, _idclub integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	UPDATE pangya.pangya_user_equip
         SET 
            ball_type = _IDCOMET, 
            club_id = _IDCLUB
      WHERE pangya_user_equip.UID = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.usp_flush_equip_comet_clubset(_iduser integer, _idcomet integer, _idclub integer) OWNER TO postgres;

--
-- TOC entry 866 (class 1255 OID 23566)
-- Name: usp_flush_equip_item(integer, integer, integer, integer, integer, integer, integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.usp_flush_equip_item(_iduser integer, _slot1 integer, _slot2 integer, _slot3 integer, _slot4 integer, _slot5 integer, _slot6 integer, _slot7 integer, _slot8 integer, _slot9 integer, _slot10 integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	UPDATE pangya.pangya_user_equip
         SET 
            item_slot_1 = _slot1, 
            item_slot_2 = _slot2, 
            item_slot_3 = _slot3, 
            item_slot_4 = _slot4, 
            item_slot_5 = _slot5, 
            item_slot_6 = _slot6, 
            item_slot_7 = _slot7, 
            item_slot_8 = _slot8, 
            item_slot_9 = _slot9, 
            item_slot_10 = _slot10
      WHERE pangya_user_equip.UID = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.usp_flush_equip_item(_iduser integer, _slot1 integer, _slot2 integer, _slot3 integer, _slot4 integer, _slot5 integer, _slot6 integer, _slot7 integer, _slot8 integer, _slot9 integer, _slot10 integer) OWNER TO postgres;

--
-- TOC entry 867 (class 1255 OID 23567)
-- Name: usp_flush_equip_mascot(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.usp_flush_equip_mascot(_iduser integer, _idmascot integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	UPDATE pangya.pangya_user_equip
         SET 
            mascot_id = _IDMASCOT
      WHERE pangya_user_equip.UID = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.usp_flush_equip_mascot(_iduser integer, _idmascot integer) OWNER TO postgres;

--
-- TOC entry 868 (class 1255 OID 23568)
-- Name: usp_flush_equip_poster(integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.usp_flush_equip_poster(_iduser integer, _poster1 integer, _poster2 integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	UPDATE pangya.pangya_user_equip
         SET 
            poster_1 = _poster1, 
            poster_2 = _poster2
      WHERE pangya_user_equip.UID = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.usp_flush_equip_poster(_iduser integer, _poster1 integer, _poster2 integer) OWNER TO postgres;

--
-- TOC entry 869 (class 1255 OID 23569)
-- Name: usp_flush_gamepoint(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.usp_flush_gamepoint(_iduser integer, _experience integer) RETURNS TABLE("_LEVEL_" smallint, "_XP_" bigint)
    LANGUAGE plpgsql
    AS $$
BEGIN
	UPDATE pangya.user_info
		SET 
			xp = user_info.Xp + _EXPERIENCE
	WHERE user_info.UID = _IDUSER;

	RETURN QUERY SELECT user_info.level, user_info.Xp
	FROM pangya.user_info
	WHERE user_info.UID = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.usp_flush_gamepoint(_iduser integer, _experience integer) OWNER TO postgres;

--
-- TOC entry 870 (class 1255 OID 23570)
-- Name: usp_flush_mascot(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.usp_flush_mascot(_iduser integer, _idmascot integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	UPDATE pangya.pangya_user_equip
         SET 
            mascot_id = _IDMASCOT
      WHERE pangya_user_equip.UID = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.usp_flush_mascot(_iduser integer, _idmascot integer) OWNER TO postgres;

--
-- TOC entry 871 (class 1255 OID 23571)
-- Name: usp_flush_moedas(integer, bigint, bigint); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.usp_flush_moedas(_iduser integer, _mpang bigint, _mcookie bigint) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	UPDATE pangya.user_info
         SET 
            Pang = _mPang, 
            Cookie = _mCookie
      WHERE user_info.UID = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.usp_flush_moedas(_iduser integer, _mpang bigint, _mcookie bigint) OWNER TO postgres;

--
-- TOC entry 872 (class 1255 OID 23572)
-- Name: usp_flush_skin(integer, integer, integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.usp_flush_skin(_iduser integer, _iskin_1 integer, _iskin_2 integer, _iskin_3 integer, _iskin_4 integer, _iskin_5 integer, _iskin_6 integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	UPDATE pangya.pangya_user_equip
         SET 
            Skin_1 = _iSKIN_1, 
            Skin_2 = _iSKIN_2, 
            Skin_3 = _iSKIN_3, 
            Skin_4 = _iSKIN_4, 
            Skin_5 = _iSKIN_5, 
            Skin_6 = _iSKIN_6
      WHERE pangya_user_equip.UID = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.usp_flush_skin(_iduser integer, _iskin_1 integer, _iskin_2 integer, _iskin_3 integer, _iskin_4 integer, _iskin_5 integer, _iskin_6 integer) OWNER TO postgres;

--
-- TOC entry 858 (class 1255 OID 23573)
-- Name: usp_flush_stat_clubset(integer, integer, integer, integer, integer, integer, integer, bigint); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.usp_flush_stat_clubset(_iduser integer, _iditem integer, _c_0 integer, _c_1 integer, _c_2 integer, _c_3 integer, _c_4 integer, _pang_in bigint) RETURNS void
    LANGUAGE plpgsql
    AS $$
DECLARE
	_item int = 0;
BEGIN
	SELECT pangya_clubset_enchant.item_id INTO _item
	FROM pangya.pangya_clubset_enchant
	WHERE pangya_clubset_enchant.uid = _IDUSER AND pangya_clubset_enchant.item_id = _IDITEM;
	
	IF _item IS NULL OR _item = 0 THEN

		INSERT INTO pangya.pangya_clubset_enchant(
			pangya.pangya_clubset_enchant.uid, 
			pangya.pangya_clubset_enchant.item_id, 
			pangya.pangya_clubset_enchant.pang, 
			pangya.pangya_clubset_enchant.c0, 
			pangya.pangya_clubset_enchant.c1, 
			pangya.pangya_clubset_enchant.c2, 
			pangya.pangya_clubset_enchant.c3, 
			pangya.pangya_clubset_enchant.c4)
		VALUES (
			_IDUSER, 
			_IDITEM, 
			0, 
			0, 
			0, 
			0, 
			0, 
			0);
	END IF;

	UPDATE pangya.pangya_clubset_enchant
		SET 
			C0 = pangya_clubset_enchant.c0 + _C_0, 
			C1 = pangya_clubset_enchant.c1 + _C_1, 
			C2 = pangya_clubset_enchant.c2 + _C_2, 
			C3 = pangya_clubset_enchant.c3 + _C_3, 
			C4 = pangya_clubset_enchant.c4 + _C_4, 
			pang = pangya_clubset_enchant.pang + _PANG_IN
	WHERE pangya_clubset_enchant.uid = _IDUSER AND pangya_clubset_enchant.item_id = _IDITEM;

	UPDATE pangya.user_info
		SET 
			pang = user_info.Pang - _PANG_IN
			WHERE user_info.UID = _IDUSER;
	END;
$$;


ALTER FUNCTION pangya.usp_flush_stat_clubset(_iduser integer, _iditem integer, _c_0 integer, _c_1 integer, _c_2 integer, _c_3 integer, _c_4 integer, _pang_in bigint) OWNER TO postgres;

--
-- TOC entry 873 (class 1255 OID 23574)
-- Name: usp_flush_tiki_points(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.usp_flush_tiki_points(_iduser integer, _tiki_pts integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	UPDATE pangya.pangya_tiki_points
         SET 
            Tiki_Points = pangya_tiki_points.Tiki_Points + _TIKI_PTS, 
            MOD_DATE = now()
      WHERE pangya_tiki_points.UID = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.usp_flush_tiki_points(_iduser integer, _tiki_pts integer) OWNER TO postgres;

--
-- TOC entry 874 (class 1255 OID 23575)
-- Name: usp_flushcharactercutin(integer, integer, integer, integer, integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.usp_flushcharactercutin(_iduser integer, _character_id integer, _cutin_typeid_1 integer, _cutin_typeid_2 integer, _cutin_typeid_3 integer, _cutin_typeid_4 integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	UPDATE pangya.pangya_character_information
         SET 
            CutIn_1 = _CUTIN_TYPEID_1,
            CutIn_2 = _CUTIN_TYPEID_2,
            CutIn_3 = _CUTIN_TYPEID_3,
            CutIn_4 = _CUTIN_TYPEID_4
      WHERE pangya_character_information.UID = _IDUSER AND pangya_character_information.item_id = _CHARACTER_ID;
END;
$$;


ALTER FUNCTION pangya.usp_flushcharactercutin(_iduser integer, _character_id integer, _cutin_typeid_1 integer, _cutin_typeid_2 integer, _cutin_typeid_3 integer, _cutin_typeid_4 integer) OWNER TO postgres;

--
-- TOC entry 875 (class 1255 OID 23576)
-- Name: usp_item_active_att(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.usp_item_active_att(_iduser integer, _itemtypeid integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	IF 
	(
		SELECT pangya_user_equip.item_slot_1
		FROM pangya.pangya_user_equip
		WHERE pangya_user_equip.UID = _IDUSER AND pangya_user_equip.item_slot_1 = _ITEMTYPEID
	) IS NOT NULL THEN
		UPDATE pangya.pangya_user_equip
			SET 
				item_slot_1 = 0
		WHERE pangya_user_equip.UID = _IDUSER;
	ELSIF 
	(
		SELECT pangya_user_equip.item_slot_2
		FROM pangya.pangya_user_equip
		WHERE pangya_user_equip.UID = _IDUSER AND pangya_user_equip.item_slot_2 = _ITEMTYPEID
	) IS NOT NULL THEN
		UPDATE pangya.pangya_user_equip
			SET 
				item_slot_2 = 0
		WHERE pangya_user_equip.UID = _IDUSER;
	ELSIF 
	(
		SELECT pangya_user_equip.item_slot_3
		FROM pangya.pangya_user_equip
		WHERE pangya_user_equip.UID = _IDUSER AND pangya_user_equip.item_slot_3 = _ITEMTYPEID
	) IS NOT NULL THEN
		UPDATE pangya.pangya_user_equip
			SET 
				item_slot_3 = 0
		WHERE pangya_user_equip.UID = _IDUSER;
	ELSIF 
	(
		SELECT pangya_user_equip.item_slot_4
		FROM pangya.pangya_user_equip
		WHERE pangya_user_equip.UID = _IDUSER AND pangya_user_equip.item_slot_4 = _ITEMTYPEID
	) IS NOT NULL THEN
		UPDATE pangya.pangya_user_equip
			SET 
				item_slot_4 = 0
		WHERE pangya_user_equip.UID = _IDUSER;
	ELSIF 
	(
		SELECT pangya_user_equip.item_slot_5
		FROM pangya.pangya_user_equip
		WHERE pangya_user_equip.UID = _IDUSER AND pangya_user_equip.item_slot_5 = _ITEMTYPEID
	) IS NOT NULL THEN
		UPDATE pangya.pangya_user_equip
			SET 
				item_slot_5 = 0
		WHERE pangya_user_equip.UID = _IDUSER;
	ELSIF 
	(
		SELECT pangya_user_equip.item_slot_6
		FROM pangya.pangya_user_equip
		WHERE pangya_user_equip.UID = _IDUSER AND pangya_user_equip.item_slot_6 = _ITEMTYPEID
	) IS NOT NULL THEN
		UPDATE pangya.pangya_user_equip
			SET 
				item_slot_6 = 0
		WHERE pangya_user_equip.UID = _IDUSER;
	ELSIF 
	(
		SELECT pangya_user_equip.item_slot_7
		FROM pangya.pangya_user_equip
		WHERE pangya_user_equip.UID = _IDUSER AND pangya_user_equip.item_slot_7 = _ITEMTYPEID
	) IS NOT NULL THEN
		UPDATE pangya.pangya_user_equip
			SET 
				item_slot_7 = 0
		WHERE pangya_user_equip.UID = _IDUSER;
	ELSIF 
	(
		SELECT pangya_user_equip.item_slot_8
		FROM pangya.pangya_user_equip
		WHERE pangya_user_equip.UID = _IDUSER AND pangya_user_equip.item_slot_8 = _ITEMTYPEID
	) IS NOT NULL THEN
		UPDATE pangya.pangya_user_equip
			SET 
				item_slot_8 = 0
		WHERE pangya_user_equip.UID = _IDUSER;
	ELSIF 
	(
		SELECT pangya_user_equip.item_slot_9
		FROM pangya.pangya_user_equip
		WHERE pangya_user_equip.UID = _IDUSER AND pangya_user_equip.item_slot_9 = _ITEMTYPEID
	) IS NOT NULL THEN
		UPDATE pangya.pangya_user_equip
			SET 
				item_slot_9 = 0
		WHERE pangya_user_equip.UID = _IDUSER;
	ELSIF 
	(
		SELECT pangya_user_equip.item_slot_10
		FROM pangya.pangya_user_equip
		WHERE pangya_user_equip.UID = _IDUSER AND pangya_user_equip.item_slot_10 = _ITEMTYPEID
	) IS NOT NULL THEN
		UPDATE pangya.pangya_user_equip
			SET 
				item_slot_10 = 0
		WHERE pangya_user_equip.UID = _IDUSER;
	END IF;

	SELECT pangya_item_warehouse.item_id
	FROM pangya.pangya_item_warehouse
	WHERE pangya_item_warehouse.UID = _IDUSER AND pangya_item_warehouse.typeid = _ITEMTYPEID;
END;
$$;


ALTER FUNCTION pangya.usp_item_active_att(_iduser integer, _itemtypeid integer) OWNER TO postgres;

--
-- TOC entry 876 (class 1255 OID 23577)
-- Name: usp_level_reward(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.usp_level_reward(_iduser integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
DECLARE 
	--#Level Reward Player, Send Prize For Mail
	_EXP INT default 0;
	_MSGID INT default 0;
	_LVL INT;
BEGIN
	SELECT level INTO _LVL FROM user_info WHERE UID = _IDUSER;

	IF _LVL != 0 THEN

		INSERT INTO pangya_gift_table(UID, FROMID, MESSAGE, GIFTDATE, LIDA_YN, VALID)
				VALUES(_IDUSER, '_SUPER', 'Rewards Level Up', now(), 0, 1);

		SELECT MAX(MSG_ID) INTO _MSGID FROM pangya_gift_table;

		IF _LVL = 1 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 402653192, 0, 10, 0);
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 402653191, 0, 10, 0);
		ELSIF _LVL = 2 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 402653189, 0, 10, 0);
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 402653188, 0, 5, 0);
		ELSIF _LVL = 3 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 436207633, 0, 60, 0);
		ELSIF _LVL = 4 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 402653188, 0, 10, 0);
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 436207633, 0, 50, 0);
		ELSIF _LVL = 5 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, x'1A00000F'::int, 0, 5, 0);
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 436207633, 0, 50, 0);
		ELSIF _LVL = 6 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, x'1A000010'::int, 0, 10000, 0); 
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 436207656, 0, 5, 0); 
		ELSIF _LVL = 7 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 402653200, 0, 5, 0); 
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 402653190, 0, 5, 0); 
		ELSIF _LVL = 8 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 402653202, 0, 5, 0); 
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 402653198, 0, 5, 0); 
		ELSIF _LVL = 9 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 402653201, 0, 5, 0); 
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 402653223, 0, 5, 0); 
		ELSIF _LVL = 10 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 436207618, 0, 20, 0); 
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 402653224, 0, 5, 0); 
		ELSIF _LVL = 11 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, x'1A000010'::int, 0, 30000, 0); 
		ELSIF _LVL = 12 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 436207667, 0, 1, 0); 
		ELSIF _LVL = 13 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 1073741827, 1, 1, 1); 
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 335544321, 0, 18, 0); 
		ELSIF _LVL = 14 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 1073741826, 1, 1, 1); 
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 335544322, 0, 36, 0); 
		ELSIF _LVL = 15 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 1073741825, 1, 1, 1); 
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 436207877, 0, 1, 0); 
		ELSIF _LVL = 16 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, x'1A000010'::int, 0, 50000, 0); 
		ELSIF _LVL = 17 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 2092957699, 0, 3, 0); 
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 436207680, 0, 50, 0); 
		ELSIF _LVL = 18 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 2092957698, 0, 2, 0); 
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 436207680, 0, 50, 0); 
		ELSIF _LVL = 19 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 2092957697, 0, 1, 0); 
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 436207680, 0, 50, 0); 
		ELSIF _LVL = 20 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 2092957696, 0, 1, 0); 
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 2092957700, 0, 1, 0); 
		ELSIF _LVL = 21 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, x'1A000010'::int, 0, 70000, 0); 
		ELSIF _LVL = 22 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 436207710, 0, 1, 0); 
		ELSIF _LVL = 23 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 436207709, 0, 1, 0); 
		ELSIF _LVL = 24 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 436207708, 0, 1, 0); 
		ELSIF _LVL = 25 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 436207707, 0, 1, 0); 
		ELSIF _LVL = 26 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, x'1A000010'::int, 0, 90000, 0); 
		ELSIF _LVL = 27 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, x'1A0000F7'::int, 0, 1, 0); 
		ELSIF _LVL = 28 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, x'1A0000F7'::int, 0, 1, 0); 
		ELSIF _LVL = 29 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, x'1A0000F7'::int, 0, 1, 0); 
		ELSIF _LVL = 30 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, x'1A0000F7'::int, 0, 1, 0); 
		ELSIF _LVL = 31 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, x'1A000010'::int, 0, 110000, 0); 
		ELSIF _LVL = 32 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 2092957699, 0, 4, 0); 
		ELSIF _LVL = 33 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 2092957698, 0, 3, 0); 
		ELSIF _LVL = 34 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 2092957697, 0, 2, 0); 
		ELSIF _LVL = 35 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, x'1A0000F7'::int, 0, 2, 0); 
		ELSIF _LVL = 36 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, x'1A000010'::int, 0, 130000, 0); 
		ELSIF _LVL = 37 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 1073741826, 1, 1, 3); 
		ELSIF _LVL = 38 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 2092957698, 0, 5, 0); 
		ELSIF _LVL = 39 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, x'1A0000F7'::int, 0, 2, 0); 
		ELSIF _LVL = 40 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 1073741825, 1, 1, 3); 
		ELSIF _LVL = 41 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, x'1A000010'::int, 0, 150000, 0); 
		ELSIF _LVL = 42 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 1073741826, 1, 1, 5); 
		ELSIF _LVL = 43 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 2092957697, 0, 3, 0); 
		ELSIF _LVL = 44 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, x'1A0000F7'::int, 0, 2, 0); 
		ELSIF _LVL = 45 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 1073741825, 1, 1, 5); 
		ELSIF _LVL = 46 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, x'1A000010'::int, 0, 170000, 0); 
		ELSIF _LVL = 47 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 436207667, 0, 3, 0); 
		ELSIF _LVL = 48 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 436207667, 0, 5, 0); 
		ELSIF _LVL = 49 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 436207667, 0, 7, 0); 
		ELSIF _LVL = 50 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 436207667, 0, 10, 0); 
		ELSIF _LVL = 51 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, x'1A000010'::int, 0, 190000, 0); 
		ELSIF _LVL = 52 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, x'1A0000F7'::int, 0, 2, 0); 
		ELSIF _LVL = 53 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, x'1A0000F7'::int, 0, 3, 0); 
		ELSIF _LVL = 54 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, x'1A0000F7'::int, 0, 4, 0); 
		ELSIF _LVL = 55 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 1073741826, 1, 1, 7); 
		ELSIF _LVL = 56 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, x'1A000010'::int, 0, 210000, 0); 
		ELSIF _LVL = 57 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 436207667, 0, 5, 0); 
		ELSIF _LVL = 58 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 436207667, 0, 10, 0); 
		ELSIF _LVL = 59 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 436207667, 0, 15, 0); 
		ELSIF _LVL = 60 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, 1073741825, 1, 1, 15); 
		ELSIF _LVL = 61 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, x'1A000010'::int, 0, 230000, 0); 
		ELSIF _LVL = 62 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, x'1A000010'::int, 0, 250000, 0); 
		ELSIF _LVL = 63 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, x'1A000010'::int, 0, 300000, 0); 
		ELSIF _LVL = 64 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, x'1A000010'::int, 0, 350000, 0); 
		ELSIF _LVL = 65 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, x'1A000010'::int, 0, 400000, 0); 
		ELSIF _LVL = 66 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, x'1A000010'::int, 0, 6000000, 0); 
		ELSIF _LVL = 67 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, x'1A000010'::int, 0, 700000, 0); 
		ELSIF _LVL = 68 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, x'1A000010'::int, 0, 800000, 0); 
		ELSIF _LVL = 69 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, x'1A000010'::int, 0, 900000, 0); 
		ELSIF _LVL = 70 THEN
			perform pangya.ProcInsertItemNoEmail(0, _IDUSER, MSGID, -1, x'1A000010'::int, 0, 1000000, 0);
        END IF;

	END IF;
END;
$$;


ALTER FUNCTION pangya.usp_level_reward(_iduser integer) OWNER TO postgres;

--
-- TOC entry 877 (class 1255 OID 23578)
-- Name: usp_save_macros(integer, text, text, text, text, text, text, text, text, text, text); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.usp_save_macros(_iduser integer, _macro_1 text, _macro_2 text, _macro_3 text, _macro_4 text, _macro_5 text, _macro_6 text, _macro_7 text, _macro_8 text, _macro_9 text, _macro_10 text) RETURNS void
    LANGUAGE plpgsql
    AS $$
BEGIN
	UPDATE pangya.pangya_user_macro
         SET 
            Macro1 = 
               CASE 
                  WHEN _Macro_1 IS NOT NULL THEN _Macro_1
                  ELSE 'Pangya!'
               END, 
            Macro2 = 
               CASE 
                  WHEN _Macro_2 IS NOT NULL THEN _Macro_2
                  ELSE 'Pangya!'
               END, 
            Macro3 = 
               CASE 
                  WHEN _Macro_3 IS NOT NULL THEN _Macro_3
                  ELSE 'Pangya!'
               END, 
            Macro4 = 
               CASE 
                  WHEN _Macro_4 IS NOT NULL THEN _Macro_4
                  ELSE 'Pangya!'
               END, 
            Macro5 = 
               CASE 
                  WHEN _Macro_5 IS NOT NULL THEN _Macro_5
                  ELSE 'Pangya!'
               END, 
            Macro6 = 
               CASE 
                  WHEN _Macro_6 IS NOT NULL THEN _Macro_6
                  ELSE 'Pangya!'
               END, 
            Macro7 = 
               CASE 
                  WHEN _Macro_7 IS NOT NULL THEN _Macro_7
                  ELSE 'Pangya!'
               END, 
            Macro8 = 
               CASE 
                  WHEN _Macro_8 IS NOT NULL THEN _Macro_8
                  ELSE 'Pangya!'
               END, 
            Macro9 = 
               CASE 
                  WHEN _Macro_9 IS NOT NULL THEN _Macro_9
                  ELSE 'Pangya!'
               END, 
            Macro10 = 
               CASE 
                  WHEN _Macro_10 IS NOT NULL THEN _Macro_10
                  ELSE 'Pangya!'
               END
      WHERE pangya_user_macro.UID = _IDUSER;
END;
$$;


ALTER FUNCTION pangya.usp_save_macros(_iduser integer, _macro_1 text, _macro_2 text, _macro_3 text, _macro_4 text, _macro_5 text, _macro_6 text, _macro_7 text, _macro_8 text, _macro_9 text, _macro_10 text) OWNER TO postgres;

--
-- TOC entry 878 (class 1255 OID 23579)
-- Name: usp_save_room(integer, integer, integer, integer, numeric, numeric, numeric, numeric, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.usp_save_room(_iduser integer, _iditem integer, _idtype integer, _no_room integer, _x numeric, _y numeric, _z numeric, _r numeric, _yn_use integer) RETURNS void
    LANGUAGE plpgsql
    AS $$
DECLARE
	_Contador int; 
	_Contador2 int;
BEGIN
	SELECT count(*) INTO _Contador
	FROM pangya.td_room_data
	WHERE td_room_data.UID = _IDUSER AND td_room_data.MYROOM_ID = _IDITEM;

	SELECT count(*) INTO _Contador2
	FROM pangya.td_room_data;
	
	IF _Contador > 0 THEN
		UPDATE pangya.td_room_data
			SET 
				ROOM_NO = _NO_ROOM, 
				POS_X = _X, 
				POS_Y = _Y, 
				POS_Z = _Z, 
				POS_R = _R, 
				DISPLAY_YN = 
					CASE 
					WHEN _YN_USE = 1 THEN 'Y'
					ELSE 'N'
					END
		WHERE td_room_data.UID = _IDUSER AND td_room_data.MYROOM_ID = _IDITEM;
	ELSIF _IDITEM IS NULL OR _IDITEM = 0 THEN
		INSERT INTO pangya.td_room_data(
			pangya.td_room_data.UID, 
			pangya.td_room_data.ROOM_NO, 
			pangya.td_room_data.TYPEID, 
			pangya.td_room_data.POS_X, 
			pangya.td_room_data.POS_Y, 
			pangya.td_room_data.POS_Z, 
			pangya.td_room_data.POS_R, 
			pangya.td_room_data.MOD_SEQ, 
			pangya.td_room_data.DISPLAY_YN, 
			pangya.td_room_data.MOD_DT)
		VALUES (
			_IDUSER, 
			_NO_ROOM, 
			_IDTYPE, 
			_X, 
			_Y, 
			_Z, 
			_R, 
			_Contador2 + 1, 
			'Y', 
			now());
	ELSE

		INSERT INTO pangya.td_room_data(
			pangya.td_room_data.UID, 
			pangya.td_room_data.ROOM_NO, 
			pangya.td_room_data.TYPEID, 
			pangya.td_room_data.MYROOM_ID, 
			pangya.td_room_data.POS_X, 
			pangya.td_room_data.POS_Y, 
			pangya.td_room_data.POS_Z, 
			pangya.td_room_data.POS_R, 
			pangya.td_room_data.MOD_SEQ, 
			pangya.td_room_data.DISPLAY_YN, 
			pangya.td_room_data.MOD_DT)
		OVERRIDING SYSTEM VALUE
		VALUES (
			_IDUSER, 
			_NO_ROOM, 
			_IDTYPE, 
			_IDITEM, 
			_X, 
			_Y, 
			_Z, 
			_R, 
			_Contador2 + 1, 
			'Y', 
			now());
	END IF;
END;
$$;


ALTER FUNCTION pangya.usp_save_room(_iduser integer, _iditem integer, _idtype integer, _no_room integer, _x numeric, _y numeric, _z numeric, _r numeric, _yn_use integer) OWNER TO postgres;

--
-- TOC entry 879 (class 1255 OID 23580)
-- Name: usp_update_guild_ranking(); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.usp_update_guild_ranking() RETURNS void
    LANGUAGE plpgsql
    AS $$
DECLARE
	_GUID INT;
	_RANK INT = 1;
	tmp CURSOR FOR
		SELECT 
			GUILD_UID 
		FROM pangya.pangya_guild
		WHERE GUILD_POINT <> 0 AND GUILD_PANG <> 0 AND (GUILD_WIN <> 0 OR GUILD_LOSE <> 0 OR GUILD_DRAW <> 0)
			AND (GUILD_STATE NOT IN(4, 5) OR GUILD_CLOSURE_DATE IS NULL OR now() < GUILD_CLOSURE_DATE)
		ORDER BY GUILD_WIN DESC, GUILD_LOSE, GUILD_DRAW, GUILD_POINT DESC, GUILD_PANG DESC;
BEGIN
	OPEN tmp;
	FETCH NEXT FROM tmp INTO _GUID;
	WHILE found LOOP

		IF (SELECT GUILD_UID FROM pangya.pangya_guild_ranking WHERE GUILD_UID = _GUID) IS NOT NULL THEN
		
			-- Já tem a Guild no Rank Atualiza
			UPDATE pangya.pangya_guild_ranking
				SET LAST_RANK = RANK,
					RANK = _RANK,
					REG_DATE = now()
			WHERE GUILD_UID = _GUID;
		ELSE

			-- Não tem a Guild no Rank Adiciona
			INSERT INTO pangya.pangya_guild_ranking(GUILD_UID, RANK, LAST_RANK, REG_DATE)
			VALUES(_GUID, _RANK, -1/*LAST RANK*/, now());
		END IF;

		-- SOMA RANK + 1
		_RANK = _RANK + 1;

		-- FETCH NEXT
		FETCH NEXT FROM tmp INTO _GUID;
	END LOOP;

	CLOSE tmp;
END;
$$;


ALTER FUNCTION pangya.usp_update_guild_ranking() OWNER TO postgres;

--
-- TOC entry 880 (class 1255 OID 23581)
-- Name: verificacoupondesconto(integer, integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya.verificacoupondesconto(_iduser integer, _itemid integer) RETURNS TABLE("_VALOR_" integer)
    LANGUAGE plpgsql
    AS $$
DECLARE
	_TIPOID int = 0;
BEGIN
	SELECT pangya_item_warehouse.typeid INTO _TIPOID
	FROM pangya.pangya_item_warehouse
	WHERE pangya_item_warehouse.UID = _IDUSER AND pangya_item_warehouse.item_id = _ITEMID;
	
	_TIPOID = 
         CASE 
            WHEN 
               (
                  SELECT pangya_coupon_desconto.typeid
                  FROM pangya.pangya_coupon_desconto
                  WHERE pangya_coupon_desconto.typeid = _TIPOID
               ) IS NULL THEN 0
            ELSE _TIPOID
         END;
		 
	IF _TIPOID = 0 THEN
		RETURN QUERY SELECT _TIPOID AS valor;
	ELSIF _TIPOID = 436207676 THEN
		RETURN QUERY SELECT 1::int AS valor;
	ELSE 
		RETURN QUERY SELECT pangya_coupon_desconto.valor
		FROM pangya.pangya_coupon_desconto
		WHERE pangya_coupon_desconto.typeid = _TIPOID;
	END IF;
END;
$$;


ALTER FUNCTION pangya.verificacoupondesconto(_iduser integer, _itemid integer) OWNER TO postgres;

--
-- TOC entry 881 (class 1255 OID 23582)
-- Name: verificawarehouseitemtime$impl(integer); Type: FUNCTION; Schema: pangya; Owner: postgres
--

CREATE FUNCTION pangya."verificawarehouseitemtime$impl"(_iduser integer, OUT _returnvalue integer) RETURNS integer
    LANGUAGE plpgsql
    AS $$
BEGIN
	_returnvalue = null::int;
	
	UPDATE pangya.pangya_item_warehouse
		SET 
			valid = 0
	WHERE 
		pangya_item_warehouse.UID = _IDUSER AND 
		pangya_item_warehouse.flag <> 96 AND 
		pangya_item_warehouse.Applytime <> pangya_item_warehouse.EndDate AND
		datediff('second', now()::timestamp, EndDate) <= 0;

	_returnvalue = 1::int;
END;
$$;


ALTER FUNCTION pangya."verificawarehouseitemtime$impl"(_iduser integer, OUT _returnvalue integer) OWNER TO postgres;

--
-- TOC entry 882 (class 1255 OID 23583)
-- Name: UNIX_TIMESTAMP(anyelement); Type: FUNCTION; Schema: public; Owner: postgres
--

CREATE FUNCTION public."UNIX_TIMESTAMP"(_date anyelement) RETURNS bigint
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN extract(epoch from _date);
END;
$$;


ALTER FUNCTION public."UNIX_TIMESTAMP"(_date anyelement) OWNER TO postgres;

--
-- TOC entry 883 (class 1255 OID 23584)
-- Name: datediff(character varying, anyelement, anyelement); Type: FUNCTION; Schema: public; Owner: postgres
--

CREATE FUNCTION public.datediff(units character varying, start_t anyelement, end_t anyelement) RETURNS bigint
    LANGUAGE plpgsql
    AS $$
DECLARE
     diff_interval INTERVAL; 
     diff bigint = 0;
     years_diff bigint = 0;
   BEGIN
   	 -- to lower case
	 units = lower(units);
	 
     IF units IN ('yy', 'yyyy', 'year', 'mm', 'm', 'month') THEN
       years_diff = DATE_PART('year', end_t) - DATE_PART('year', start_t);
 
       IF units IN ('yy', 'yyyy', 'year') THEN
         -- SQL Server does not count full years passed (only difference between year parts)
         RETURN years_diff;
       ELSE
         -- If end month is less than start month it will subtracted
         RETURN years_diff * 12 + (DATE_PART('month', end_t) - DATE_PART('month', start_t)); 
       END IF;
     END IF;
 
     -- Minus operator returns interval 'DDD days HH:MI:SS'  
     diff_interval = end_t - start_t;
 
     diff = diff + DATE_PART('day', diff_interval);
 
     IF units IN ('wk', 'ww', 'week') THEN
       diff = diff/7;
       RETURN diff;
     END IF;
 
     IF units IN ('dd', 'd', 'day') THEN
       RETURN diff;
     END IF;
 
     diff = diff * 24 + DATE_PART('hour', diff_interval); 
 
     IF units IN ('hh', 'hour') THEN
        RETURN diff;
     END IF;
 
     diff = diff * 60 + DATE_PART('minute', diff_interval);
 
     IF units IN ('mi', 'n', 'minute') THEN
        RETURN diff;
     END IF;
 
     diff = diff * 60 + DATE_PART('second', diff_interval);
 
     RETURN diff;
   END;
$$;


ALTER FUNCTION public.datediff(units character varying, start_t anyelement, end_t anyelement) OWNER TO postgres;

--
-- TOC entry 884 (class 1255 OID 23585)
-- Name: getachievementpoint(integer); Type: FUNCTION; Schema: public; Owner: postgres
--

CREATE FUNCTION public.getachievementpoint(_iduser integer) RETURNS bigint
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN (SELECT
			COUNT(b.id) * 10 AS "_LEVEL_"
		FROM
			pangya.pangya_achievement a
			INNER JOIN
			pangya.pangya_quest b
			ON a.ID_ACHIEVEMENT = b.achievement_id
		WHERE 
			a.UID = _IDUSER
			AND b.Date IS NOT NULL
		GROUP BY a.UID);
END;
$$;


ALTER FUNCTION public.getachievementpoint(_iduser integer) OWNER TO postgres;

--
-- TOC entry 885 (class 1255 OID 23586)
-- Name: getcardexprate(integer); Type: FUNCTION; Schema: public; Owner: postgres
--

CREATE FUNCTION public.getcardexprate(_iduser integer) RETURNS bigint
    LANGUAGE plpgsql
    AS $$
DECLARE
	_qntd_efeito int = 0;
	_qntd int = 0;
	 TYPEID_CURSOR CURSOR FOR 
            SELECT Efeito_Qntd
            FROM pangya.pangya_card_equip
            WHERE 
              UID = _IDUSER AND 
              Efeito = 3 AND 
              END_DT > now();
BEGIN
	
	OPEN TYPEID_CURSOR;
	
	<< loop_cursor >>
	WHILE (1 = 1) LOOP
	BEGIN
		FETCH NEXT FROM TYPEID_CURSOR INTO _qntd_efeito;

		IF not found THEN
			exit loop_cursor;
		END IF;

		_qntd = _qntd + _qntd_efeito;
	END;
	END LOOP;
	
	CLOSE TYPEID_CURSOR;
	
	RETURN _qntd;
END;
$$;


ALTER FUNCTION public.getcardexprate(_iduser integer) OWNER TO postgres;

--
-- TOC entry 886 (class 1255 OID 23587)
-- Name: getcardpangrate(integer); Type: FUNCTION; Schema: public; Owner: postgres
--

CREATE FUNCTION public.getcardpangrate(_iduser integer) RETURNS bigint
    LANGUAGE plpgsql
    AS $$
DECLARE
	_qntd_efeito int = 0;
	_qntd int = 0;
	 TYPEID_CURSOR CURSOR FOR 
            SELECT Efeito_Qntd
            FROM pangya.pangya_card_equip
            WHERE UID = _IDUSER 
				AND Efeito = 2 OR (Efeito = 3 AND card_typeid = 436207841) AND END_DT > now();
BEGIN
	
	OPEN TYPEID_CURSOR;
	
	<< loop_cursor >>
	WHILE (1 = 1) LOOP
	BEGIN
		FETCH NEXT FROM TYPEID_CURSOR INTO _qntd_efeito;

		IF not found THEN
			exit loop_cursor;
		END IF;

		_qntd = _qntd + _qntd_efeito;
	END;
	END LOOP;
	
	CLOSE TYPEID_CURSOR;
	
	RETURN _qntd;
END;
$$;


ALTER FUNCTION public.getcardpangrate(_iduser integer) OWNER TO postgres;

--
-- TOC entry 887 (class 1255 OID 23588)
-- Name: getitensexprate(integer); Type: FUNCTION; Schema: public; Owner: postgres
--

CREATE FUNCTION public.getitensexprate(_iduser integer) RETURNS bigint
    LANGUAGE plpgsql
    AS $$
DECLARE
	_item_typeid int = 0;
	_qntd int = 0;
	 TYPEID_CURSOR CURSOR FOR 
            SELECT typeid
            FROM pangya.pangya_item_warehouse
            WHERE 
               UID = _IDUSER AND 
               valid = 1 AND 
               C0 > 0 AND 
               typeid IN ( 
               436207626, 
               436207627, 
               436207629, 
               436207630, 
               436207631, 
               436207636, 
               436207669, 
               436207749, 
               436207769, 
               436207868 );
BEGIN
	
	OPEN TYPEID_CURSOR;
	
	<< loop_cursor >>
	WHILE (1 = 1) LOOP
	BEGIN
		FETCH NEXT FROM TYPEID_CURSOR INTO _item_typeid;

		IF not found THEN
			exit loop_cursor;
		END IF;

		_qntd = _qntd + 200;
	END;
	END LOOP;
	
	CLOSE TYPEID_CURSOR;
	
	RETURN _qntd;
END;
$$;


ALTER FUNCTION public.getitensexprate(_iduser integer) OWNER TO postgres;

--
-- TOC entry 888 (class 1255 OID 23589)
-- Name: getitenspangrate(integer); Type: FUNCTION; Schema: public; Owner: postgres
--

CREATE FUNCTION public.getitenspangrate(_iduser integer) RETURNS bigint
    LANGUAGE plpgsql
    AS $$
DECLARE
	_item_typeid int = 0;
	_qntd int = 0;
	 TYPEID_CURSOR CURSOR FOR 
            SELECT typeid
            FROM pangya.pangya_item_warehouse
            WHERE 
               UID = _IDUSER AND 
               valid = 1 AND 
               C0 > 0 AND 
               typeid IN ( 
               436207617, 
               436207618, 
               436207621, 
               436207625, 
               436207628 );
BEGIN
	
	OPEN TYPEID_CURSOR;
	
	<< loop_cursor >>
	WHILE (1 = 1) LOOP
	BEGIN
		FETCH NEXT FROM TYPEID_CURSOR INTO _item_typeid;

		IF not found THEN
			exit loop_cursor;
		END IF;

		IF _item_typeid IN ( 436207617, 436207618 ) THEN
			_qntd = _qntd + 200;
		ELSIF _item_typeid = 436207621 THEN
			_qntd = _qntd + 300;
		ELSE
			_qntd = _qntd + 20;
		END IF;
	END;
	END LOOP;
	
	CLOSE TYPEID_CURSOR;
	
	RETURN _qntd;
END;
$$;


ALTER FUNCTION public.getitenspangrate(_iduser integer) OWNER TO postgres;

--
-- TOC entry 889 (class 1255 OID 23590)
-- Name: getlevelclubset(integer, integer, integer); Type: FUNCTION; Schema: public; Owner: postgres
--

CREATE FUNCTION public.getlevelclubset(_iduser integer, _taqueira_id integer, _qntd integer) RETURNS bigint
    LANGUAGE plpgsql
    AS $$
DECLARE
	_LEVEL int = 0;
BEGIN

	SELECT (C0 + C1 + C2 + C3 + C4 + 
			ClubSet_WorkShop_C0 + ClubSet_WorkShop_C1 + 
			ClubSet_WorkShop_C2 + ClubSet_WorkShop_C3 + 
			ClubSet_WorkShop_C4 + _qntd) INTO _LEVEL 
	FROM pangya.pangya_item_warehouse
	WHERE
		UID = _IDUSER AND item_id = _taqueira_id;
	
	-- Não tem a taqueira
	IF not found THEN
		RETURN -1;
	END IF;
	
	IF _LEVEL < 30 THEN
		_LEVEL = -1;
	ELSIF _LEVEL < 35 THEN
		_LEVEL = _LEVEL - 30;
	ELSIF _LEVEL < 40 THEN
		_LEVEL = _LEVEL - 35;
	ELSIF _LEVEL < 45 THEN
		_LEVEL = _LEVEL - 40;
	ELSIF _LEVEL < 50 THEN
		_LEVEL = _LEVEL - 45;
	ELSIF _LEVEL < 55 THEN
		_LEVEL = _LEVEL - 50;
	ELSIF _LEVEL < 60 THEN
		_LEVEL = -1;
	END IF;

	RETURN _LEVEL;
END;
$$;


ALTER FUNCTION public.getlevelclubset(_iduser integer, _taqueira_id integer, _qntd integer) OWNER TO postgres;

--
-- TOC entry 890 (class 1255 OID 23591)
-- Name: getlevelmemorial(integer); Type: FUNCTION; Schema: public; Owner: postgres
--

CREATE FUNCTION public.getlevelmemorial(_iduser integer) RETURNS integer
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN null::int;
END;
$$;


ALTER FUNCTION public.getlevelmemorial(_iduser integer) OWNER TO postgres;

--
-- TOC entry 891 (class 1255 OID 23592)
-- Name: getlevelmemorial(integer, integer, integer); Type: FUNCTION; Schema: public; Owner: postgres
--

CREATE FUNCTION public.getlevelmemorial(_iduser integer, _taqueira_id integer, _qntd integer) RETURNS bigint
    LANGUAGE plpgsql
    AS $$
DECLARE
	_LEVEL int = 0;
BEGIN
	
	SELECT (C0 + C1 + C2 + C3 + C4 + 
			ClubSet_WorkShop_C0 + ClubSet_WorkShop_C1 + 
			ClubSet_WorkShop_C2 + ClubSet_WorkShop_C3 + 
			ClubSet_WorkShop_C4 + _qntd) INTO _LEVEL 
	FROM pangya.pangya_item_warehouse
	WHERE
		UID = _IDUSER AND item_id = _taqueira_id;
	
	-- Não tem a taqueira
	IF not found THEN
		RETURN -1;
	END IF;
	
	IF _LEVEL < 30 THEN
		_LEVEL = -1;
	ELSIF _LEVEL < 35 THEN
		_LEVEL = 0;
	ELSIF _LEVEL < 40 THEN
		_LEVEL = 1;
	ELSIF _LEVEL < 45 THEN
		_LEVEL = 2;
	ELSIF _LEVEL < 50 THEN
		_LEVEL = 3;
	ELSIF _LEVEL < 55 THEN
		_LEVEL = 4;
	ELSIF _LEVEL < 60 THEN
		_LEVEL = 5;
	END IF;

	RETURN _LEVEL;
END;
$$;


ALTER FUNCTION public.getlevelmemorial(_iduser integer, _taqueira_id integer, _qntd integer) OWNER TO postgres;

--
-- TOC entry 892 (class 1255 OID 23593)
-- Name: groupid(integer); Type: FUNCTION; Schema: public; Owner: postgres
--

CREATE FUNCTION public.groupid(_item_typeid integer) RETURNS integer
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN (_item_typeid & x'FF000000'::int) >> 24;
END;
$$;


ALTER FUNCTION public.groupid(_item_typeid integer) OWNER TO postgres;

--
-- TOC entry 893 (class 1255 OID 23594)
-- Name: iff(boolean, anyelement, anyelement); Type: FUNCTION; Schema: public; Owner: postgres
--

CREATE FUNCTION public.iff(condition boolean, true_result anyelement, false_result anyelement) RETURNS anyelement
    LANGUAGE sql IMMUTABLE
    AS $$
	SELECT CASE WHEN condition THEN true_result ELSE false_result END;
$$;


ALTER FUNCTION public.iff(condition boolean, true_result anyelement, false_result anyelement) OWNER TO postgres;

--
-- TOC entry 894 (class 1255 OID 23595)
-- Name: reset_all_tables_registry(); Type: FUNCTION; Schema: public; Owner: postgres
--

CREATE FUNCTION public.reset_all_tables_registry() RETURNS void
    LANGUAGE plpgsql
    AS $$
DECLARE
	_value record;
BEGIN
	FOR _value in (SELECT * from information_schema.tables WHERE table_schema = 'pangya') LOOP
		EXECUTE 'DELETE FROM ' || _value.table_schema || '.' || _value.table_name;
		RAISE NOTICE '%.%', _value.table_schema, _value.table_name;
	END LOOP;
END;
$$;


ALTER FUNCTION public.reset_all_tables_registry() OWNER TO postgres;

--
-- TOC entry 895 (class 1255 OID 23596)
-- Name: time_to_sec(anyelement); Type: FUNCTION; Schema: public; Owner: postgres
--

CREATE FUNCTION public.time_to_sec(_time anyelement) RETURNS integer
    LANGUAGE plpgsql
    AS $$
BEGIN
	RETURN DATE_PART('HOUR', _time) * 3600 +
               DATE_PART('MINUTE', _time) * 60 + DATE_PART('SECOND', _time);
END;
$$;


ALTER FUNCTION public.time_to_sec(_time anyelement) OWNER TO postgres;

SET default_tablespace = '';

SET default_table_access_method = heap;

--
-- TOC entry 209 (class 1259 OID 23597)
-- Name: account; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.account (
    id character varying(25) DEFAULT ''::character varying NOT NULL,
    uid bigint NOT NULL,
    password character varying(33) DEFAULT ''::character varying NOT NULL,
    idstate bigint DEFAULT 0 NOT NULL,
    lastlogontime timestamp without time zone,
    blocktime integer DEFAULT 0 NOT NULL,
    logon smallint DEFAULT 0 NOT NULL,
    first_login smallint DEFAULT 0 NOT NULL,
    regdate timestamp without time zone,
    nick character varying(50) DEFAULT ''::character varying NOT NULL,
    first_set smallint DEFAULT 0 NOT NULL,
    guild_uid integer DEFAULT 0 NOT NULL,
    sex smallint DEFAULT 0 NOT NULL,
    dotutorial smallint DEFAULT 0 NOT NULL,
    username character varying(23) DEFAULT NULL::character varying,
    userip character varying(20) DEFAULT NULL::character varying,
    serverid character varying(20) DEFAULT NULL::character varying,
    game_server_id character varying(20) DEFAULT NULL::character varying,
    lastleavetime timestamp without time zone,
    logoncount bigint DEFAULT 0 NOT NULL,
    blockregdate timestamp without time zone,
    school integer DEFAULT 0 NOT NULL,
    capability integer DEFAULT 0 NOT NULL,
    event smallint DEFAULT 0 NOT NULL,
    mannerflag smallint DEFAULT 0 NOT NULL,
    event1 smallint DEFAULT 0 NOT NULL,
    event2 integer DEFAULT 0 NOT NULL,
    domainid integer DEFAULT 0 NOT NULL,
    channelflag smallint DEFAULT 0 NOT NULL,
    change_nick timestamp without time zone
);


ALTER TABLE pangya.account OWNER TO postgres;

--
-- TOC entry 210 (class 1259 OID 23624)
-- Name: account_uid_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.account ALTER COLUMN uid ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.account_uid_seq
    START WITH 14637
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 211 (class 1259 OID 23626)
-- Name: achievement_quest; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.achievement_quest (
    idx bigint NOT NULL,
    uid integer DEFAULT 0 NOT NULL,
    id_achievement integer DEFAULT 0 NOT NULL,
    typeid_achieve integer DEFAULT 0 NOT NULL,
    count_id integer DEFAULT 0 NOT NULL,
    data_sec integer DEFAULT 0 NOT NULL,
    objetivo_quest integer DEFAULT 0 NOT NULL
);


ALTER TABLE pangya.achievement_quest OWNER TO postgres;

--
-- TOC entry 212 (class 1259 OID 23635)
-- Name: achievement_quest_idx_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.achievement_quest ALTER COLUMN idx ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.achievement_quest_idx_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 213 (class 1259 OID 23637)
-- Name: achievement_tipo; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.achievement_tipo (
    uid integer DEFAULT 0 NOT NULL,
    nome character varying(50) DEFAULT NULL::character varying,
    typeid integer DEFAULT 0 NOT NULL,
    id_achievement bigint NOT NULL,
    tipo smallint DEFAULT 0 NOT NULL,
    option integer DEFAULT 0 NOT NULL
);


ALTER TABLE pangya.achievement_tipo OWNER TO postgres;

--
-- TOC entry 214 (class 1259 OID 23645)
-- Name: achievement_tipo_id_achievement_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.achievement_tipo ALTER COLUMN id_achievement ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.achievement_tipo_id_achievement_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 215 (class 1259 OID 23647)
-- Name: authkey_game; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.authkey_game (
    uid integer NOT NULL,
    authkey character varying(8) DEFAULT ''::character varying,
    serverid integer DEFAULT 0 NOT NULL,
    valid smallint DEFAULT 1 NOT NULL
);


ALTER TABLE pangya.authkey_game OWNER TO postgres;

--
-- TOC entry 216 (class 1259 OID 23653)
-- Name: authkey_login; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.authkey_login (
    uid integer NOT NULL,
    authkey character varying(8) DEFAULT ''::character varying NOT NULL,
    valid smallint DEFAULT 1 NOT NULL
);


ALTER TABLE pangya.authkey_login OWNER TO postgres;

--
-- TOC entry 217 (class 1259 OID 23658)
-- Name: black_papel_item; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.black_papel_item (
    index bigint NOT NULL,
    nome character varying(32) NOT NULL,
    typeid integer NOT NULL,
    numero integer DEFAULT '-1'::integer NOT NULL,
    probabilidade integer NOT NULL,
    tipo integer NOT NULL,
    flag smallint NOT NULL,
    active smallint DEFAULT 1 NOT NULL
);


ALTER TABLE pangya.black_papel_item OWNER TO postgres;

--
-- TOC entry 218 (class 1259 OID 23663)
-- Name: black_papel_item_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.black_papel_item ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.black_papel_item_index_seq
    START WITH 56
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 219 (class 1259 OID 23665)
-- Name: black_papel_prob_sec; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.black_papel_prob_sec (
    uid integer NOT NULL,
    probabilidade integer DEFAULT 0 NOT NULL
);


ALTER TABLE pangya.black_papel_prob_sec OWNER TO postgres;

--
-- TOC entry 220 (class 1259 OID 23669)
-- Name: black_papel_qntd_prob; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.black_papel_qntd_prob (
    nome character varying(50) DEFAULT NULL::character varying,
    probabilidade integer NOT NULL,
    tipo integer NOT NULL
);


ALTER TABLE pangya.black_papel_qntd_prob OWNER TO postgres;

--
-- TOC entry 221 (class 1259 OID 23673)
-- Name: black_papel_rate; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.black_papel_rate (
    nome character varying(50) DEFAULT NULL::character varying,
    probabilidade integer NOT NULL,
    tipo integer NOT NULL
);


ALTER TABLE pangya.black_papel_rate OWNER TO postgres;

--
-- TOC entry 222 (class 1259 OID 23677)
-- Name: contas_beta; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.contas_beta (
    index bigint NOT NULL,
    uid integer DEFAULT 0 NOT NULL,
    nome character varying(50) DEFAULT ''::character varying NOT NULL,
    sobre_nome character varying(100) DEFAULT ''::character varying NOT NULL,
    email character varying(100) DEFAULT ''::character varying NOT NULL,
    key_uniq uuid DEFAULT gen_random_uuid() NOT NULL,
    finish_reg smallint DEFAULT 0 NOT NULL,
    ip_register character(20) DEFAULT ''::bpchar NOT NULL,
    date_reg timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


ALTER TABLE pangya.contas_beta OWNER TO postgres;

--
-- TOC entry 223 (class 1259 OID 23688)
-- Name: contas_beta_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.contas_beta ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.contas_beta_index_seq
    START WITH 10176
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 224 (class 1259 OID 23690)
-- Name: count_item; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.count_item (
    uid integer DEFAULT 0 NOT NULL,
    nome character varying(50) DEFAULT NULL::character varying,
    id_achievement integer DEFAULT 0 NOT NULL,
    typeid integer DEFAULT 0 NOT NULL,
    count_id integer NOT NULL,
    count_num_item bigint DEFAULT 0 NOT NULL,
    data_sec integer DEFAULT 0 NOT NULL,
    tipo smallint DEFAULT 0 NOT NULL
);


ALTER TABLE pangya.count_item OWNER TO postgres;

--
-- TOC entry 225 (class 1259 OID 23700)
-- Name: count_item_count_id_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.count_item ALTER COLUMN count_id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.count_item_count_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 226 (class 1259 OID 23702)
-- Name: pangya_1st_anniversary; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_1st_anniversary (
    event_done smallint DEFAULT 0 NOT NULL,
    all_player_apt bigint DEFAULT 0 NOT NULL,
    all_player_win bigint DEFAULT 0 NOT NULL,
    reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_1st_anniversary OWNER TO postgres;

--
-- TOC entry 227 (class 1259 OID 23709)
-- Name: pangya_1st_anniversary_player_win_cp; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_1st_anniversary_player_win_cp (
    index bigint NOT NULL,
    uid integer NOT NULL,
    login_days integer DEFAULT 0 NOT NULL,
    cookie_point bigint DEFAULT 0 NOT NULL,
    reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_1st_anniversary_player_win_cp OWNER TO postgres;

--
-- TOC entry 228 (class 1259 OID 23715)
-- Name: pangya_1st_anniversary_player_win_cp_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_1st_anniversary_player_win_cp ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_1st_anniversary_player_win_cp_index_seq
    START WITH 81
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 229 (class 1259 OID 23717)
-- Name: pangya_achievement; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_achievement (
    id_achievement integer NOT NULL,
    uid integer NOT NULL,
    nome character varying(100),
    typeid integer NOT NULL,
    active integer DEFAULT 1 NOT NULL,
    status integer DEFAULT 1 NOT NULL
);


ALTER TABLE pangya.pangya_achievement OWNER TO postgres;

--
-- TOC entry 230 (class 1259 OID 23722)
-- Name: pangya_achievement_id_achievement_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_achievement ALTER COLUMN id_achievement ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_achievement_id_achievement_seq
    START WITH 73004
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 231 (class 1259 OID 23724)
-- Name: pangya_approach_missions; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_approach_missions (
    numero bigint NOT NULL,
    tipo integer DEFAULT 1 NOT NULL,
    reward_tipo integer DEFAULT 1 NOT NULL,
    box integer DEFAULT 1 NOT NULL,
    flag integer DEFAULT 0 NOT NULL,
    active smallint DEFAULT 1 NOT NULL
);


ALTER TABLE pangya.pangya_approach_missions OWNER TO postgres;

--
-- TOC entry 232 (class 1259 OID 23732)
-- Name: pangya_assistente; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_assistente (
    uid integer DEFAULT 1 NOT NULL,
    assist integer DEFAULT 0 NOT NULL
);


ALTER TABLE pangya.pangya_assistente OWNER TO postgres;

--
-- TOC entry 233 (class 1259 OID 23737)
-- Name: pangya_attendance_reward; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_attendance_reward (
    uid integer NOT NULL,
    counter integer DEFAULT 0 NOT NULL,
    item_typeid_now integer DEFAULT 0 NOT NULL,
    item_qntd_now integer DEFAULT 0 NOT NULL,
    item_typeid_after integer DEFAULT 0 NOT NULL,
    item_qntd_after integer DEFAULT 0 NOT NULL,
    last_login timestamp without time zone
);


ALTER TABLE pangya.pangya_attendance_reward OWNER TO postgres;

--
-- TOC entry 234 (class 1259 OID 23745)
-- Name: pangya_attendance_table_item_reward; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_attendance_table_item_reward (
    idx integer NOT NULL,
    nome character varying(50) DEFAULT NULL::character varying,
    typeid integer NOT NULL,
    quantidade integer NOT NULL,
    tipo integer NOT NULL
);


ALTER TABLE pangya.pangya_attendance_table_item_reward OWNER TO postgres;

--
-- TOC entry 235 (class 1259 OID 23749)
-- Name: pangya_attendance_table_item_reward_idx_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_attendance_table_item_reward ALTER COLUMN idx ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_attendance_table_item_reward_idx_seq
    START WITH 21
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 236 (class 1259 OID 23751)
-- Name: pangya_auth_key; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_auth_key (
    index bigint NOT NULL,
    server_uid integer,
    key character(16),
    valid smallint DEFAULT 1
);


ALTER TABLE pangya.pangya_auth_key OWNER TO postgres;

--
-- TOC entry 237 (class 1259 OID 23755)
-- Name: pangya_auth_key_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_auth_key ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_auth_key_index_seq
    START WITH 9
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 238 (class 1259 OID 23757)
-- Name: pangya_bot_gm_event_reward; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_bot_gm_event_reward (
    index bigint NOT NULL,
    typeid integer NOT NULL,
    qntd integer NOT NULL,
    qntd_time integer NOT NULL,
    rate integer NOT NULL,
    valid smallint DEFAULT 1 NOT NULL,
    reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_bot_gm_event_reward OWNER TO postgres;

--
-- TOC entry 239 (class 1259 OID 23762)
-- Name: pangya_bot_gm_event_reward_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_bot_gm_event_reward ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_bot_gm_event_reward_index_seq
    START WITH 27
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 240 (class 1259 OID 23764)
-- Name: pangya_bot_gm_event_time; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_bot_gm_event_time (
    index bigint NOT NULL,
    inicio_time time(6) without time zone NOT NULL,
    fim_time time(6) without time zone NOT NULL,
    channel_id smallint NOT NULL,
    valid smallint DEFAULT 0 NOT NULL,
    reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_bot_gm_event_time OWNER TO postgres;

--
-- TOC entry 241 (class 1259 OID 23769)
-- Name: pangya_bot_gm_event_time_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_bot_gm_event_time ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_bot_gm_event_time_index_seq
    START WITH 7
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 242 (class 1259 OID 23771)
-- Name: pangya_caddie_information; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_caddie_information (
    item_id bigint NOT NULL,
    uid integer NOT NULL,
    typeid integer NOT NULL,
    parts_typeid integer DEFAULT 0 NOT NULL,
    gift_flag smallint DEFAULT 0 NOT NULL,
    clevel smallint DEFAULT 0 NOT NULL,
    exp integer DEFAULT 0 NOT NULL,
    regdate timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL,
    period smallint DEFAULT 0 NOT NULL,
    enddate timestamp without time zone,
    rentflag smallint DEFAULT 1 NOT NULL,
    purchase smallint DEFAULT 0 NOT NULL,
    parts_enddate timestamp without time zone,
    checkend smallint DEFAULT 1 NOT NULL,
    valid smallint DEFAULT 1 NOT NULL
);


ALTER TABLE pangya.pangya_caddie_information OWNER TO postgres;

--
-- TOC entry 243 (class 1259 OID 23784)
-- Name: pangya_caddie_information_item_id_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_caddie_information ALTER COLUMN item_id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_caddie_information_item_id_seq
    START WITH 147
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 244 (class 1259 OID 23786)
-- Name: pangya_card; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_card (
    card_itemid bigint NOT NULL,
    uid integer NOT NULL,
    card_typeid integer NOT NULL,
    qntd integer,
    get_dt timestamp without time zone,
    use_dt timestamp without time zone,
    end_dt timestamp without time zone,
    slot integer DEFAULT 0 NOT NULL,
    efeito integer DEFAULT 0 NOT NULL,
    efeito_qntd integer DEFAULT 0 NOT NULL,
    card_type smallint DEFAULT 1 NOT NULL,
    use_yn character(1) DEFAULT NULL::bpchar
);


ALTER TABLE pangya.pangya_card OWNER TO postgres;

--
-- TOC entry 245 (class 1259 OID 23794)
-- Name: pangya_card_card_itemid_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_card ALTER COLUMN card_itemid ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_card_card_itemid_seq
    START WITH 1795
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 246 (class 1259 OID 23796)
-- Name: pangya_card_equip; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_card_equip (
    index bigint NOT NULL,
    uid integer NOT NULL,
    parts_id integer DEFAULT 0 NOT NULL,
    parts_typeid integer DEFAULT 0 NOT NULL,
    card_typeid integer NOT NULL,
    efeito integer DEFAULT 0 NOT NULL,
    efeito_qntd integer DEFAULT 0 NOT NULL,
    slot integer DEFAULT 0 NOT NULL,
    use_dt timestamp without time zone,
    end_dt timestamp without time zone,
    tipo integer DEFAULT 0 NOT NULL,
    use_yn smallint DEFAULT 1 NOT NULL,
    date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_card_equip OWNER TO postgres;

--
-- TOC entry 247 (class 1259 OID 23807)
-- Name: pangya_card_equip_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_card_equip ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_card_equip_index_seq
    START WITH 391
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 248 (class 1259 OID 23809)
-- Name: pangya_card_rate; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_card_rate (
    nome character varying(50) DEFAULT NULL::character varying,
    tipo integer NOT NULL,
    probabilidade integer NOT NULL
);


ALTER TABLE pangya.pangya_card_rate OWNER TO postgres;

--
-- TOC entry 249 (class 1259 OID 23813)
-- Name: pangya_cardpack; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_cardpack (
    nome character varying(50) DEFAULT NULL::character varying,
    typeid integer NOT NULL,
    tipo_pack integer NOT NULL,
    tipo integer NOT NULL,
    probabilidade integer NOT NULL
);


ALTER TABLE pangya.pangya_cardpack OWNER TO postgres;

--
-- TOC entry 250 (class 1259 OID 23817)
-- Name: pangya_cards; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_cards (
    nome character varying(50) DEFAULT NULL::character varying,
    typeid_pack integer NOT NULL,
    typeid_card integer NOT NULL,
    tipo integer NOT NULL
);


ALTER TABLE pangya.pangya_cards OWNER TO postgres;

--
-- TOC entry 251 (class 1259 OID 23821)
-- Name: pangya_change_nickname_log; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_change_nickname_log (
    index bigint NOT NULL,
    uid integer DEFAULT 0 NOT NULL,
    nickname character varying(50) DEFAULT ''::character varying NOT NULL,
    change_time timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_change_nickname_log OWNER TO postgres;

--
-- TOC entry 252 (class 1259 OID 23827)
-- Name: pangya_change_nickname_log_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_change_nickname_log ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_change_nickname_log_index_seq
    START WITH 19
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 253 (class 1259 OID 23829)
-- Name: pangya_change_pwd_log; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_change_pwd_log (
    uid integer NOT NULL,
    last_change timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL,
    change_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL,
    count integer DEFAULT 1 NOT NULL
);


ALTER TABLE pangya.pangya_change_pwd_log OWNER TO postgres;

--
-- TOC entry 254 (class 1259 OID 23835)
-- Name: pangya_character_information; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_character_information (
    item_id bigint NOT NULL,
    typeid integer NOT NULL,
    uid integer NOT NULL,
    parts_1 integer DEFAULT 0 NOT NULL,
    parts_2 integer DEFAULT 0 NOT NULL,
    parts_3 integer DEFAULT 0 NOT NULL,
    parts_4 integer DEFAULT 0 NOT NULL,
    parts_5 integer DEFAULT 0 NOT NULL,
    parts_6 integer DEFAULT 0 NOT NULL,
    parts_7 integer DEFAULT 0 NOT NULL,
    parts_8 integer DEFAULT 0 NOT NULL,
    parts_9 integer DEFAULT 0 NOT NULL,
    parts_10 integer DEFAULT 0 NOT NULL,
    parts_11 integer DEFAULT 0 NOT NULL,
    parts_12 integer DEFAULT 0 NOT NULL,
    parts_13 integer DEFAULT 0 NOT NULL,
    parts_14 integer DEFAULT 0 NOT NULL,
    parts_15 integer DEFAULT 0 NOT NULL,
    parts_16 integer DEFAULT 0 NOT NULL,
    parts_17 integer DEFAULT 0 NOT NULL,
    parts_18 integer DEFAULT 0 NOT NULL,
    parts_19 integer DEFAULT 0 NOT NULL,
    parts_20 integer DEFAULT 0 NOT NULL,
    parts_21 integer DEFAULT 0 NOT NULL,
    parts_22 integer DEFAULT 0 NOT NULL,
    parts_23 integer DEFAULT 0 NOT NULL,
    parts_24 integer DEFAULT 0 NOT NULL,
    default_hair smallint DEFAULT 0 NOT NULL,
    default_shirts smallint DEFAULT 0 NOT NULL,
    gift_flag smallint DEFAULT 0 NOT NULL,
    pcl0 smallint DEFAULT 0 NOT NULL,
    pcl1 smallint DEFAULT 0 NOT NULL,
    pcl2 smallint DEFAULT 0 NOT NULL,
    pcl3 smallint DEFAULT 0 NOT NULL,
    pcl4 smallint DEFAULT 0 NOT NULL,
    purchase smallint DEFAULT 0 NOT NULL,
    auxparts_1 integer DEFAULT 0 NOT NULL,
    auxparts_2 integer DEFAULT 0 NOT NULL,
    auxparts_3 integer DEFAULT 0 NOT NULL,
    auxparts_4 integer DEFAULT 0 NOT NULL,
    auxparts_5 integer DEFAULT 0 NOT NULL,
    cutin_1 integer DEFAULT 0 NOT NULL,
    cutin_2 integer DEFAULT 0 NOT NULL,
    cutin_3 integer DEFAULT 0 NOT NULL,
    cutin_4 integer DEFAULT 0 NOT NULL,
    mastery integer DEFAULT 0 NOT NULL
);


ALTER TABLE pangya.pangya_character_information OWNER TO postgres;

--
-- TOC entry 255 (class 1259 OID 23881)
-- Name: pangya_character_information_item_id_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_character_information ALTER COLUMN item_id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_character_information_item_id_seq
    START WITH 489
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 256 (class 1259 OID 23883)
-- Name: pangya_clubset_enchant; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_clubset_enchant (
    uid integer NOT NULL,
    item_id integer NOT NULL,
    pang bigint DEFAULT 0 NOT NULL,
    c0 smallint DEFAULT 0 NOT NULL,
    c1 smallint DEFAULT 0 NOT NULL,
    c2 smallint DEFAULT 0 NOT NULL,
    c3 smallint DEFAULT 0 NOT NULL,
    c4 smallint DEFAULT 0 NOT NULL
);


ALTER TABLE pangya.pangya_clubset_enchant OWNER TO postgres;

--
-- TOC entry 257 (class 1259 OID 23892)
-- Name: pangya_coin_cube_info; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_coin_cube_info (
    course_id smallint DEFAULT 0 NOT NULL,
    active smallint DEFAULT 1 NOT NULL,
    update_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_coin_cube_info OWNER TO postgres;

--
-- TOC entry 258 (class 1259 OID 23898)
-- Name: pangya_coin_cube_location; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_coin_cube_location (
    index bigint NOT NULL,
    course smallint DEFAULT 0 NOT NULL,
    hole smallint DEFAULT 0 NOT NULL,
    tipo smallint DEFAULT 0 NOT NULL,
    tipo_location smallint DEFAULT 0 NOT NULL,
    rate bigint DEFAULT 1 NOT NULL,
    x double precision DEFAULT 0.0 NOT NULL,
    y double precision DEFAULT 0.0 NOT NULL,
    z double precision DEFAULT 0.0 NOT NULL,
    reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_coin_cube_location OWNER TO postgres;

--
-- TOC entry 259 (class 1259 OID 23910)
-- Name: pangya_coin_cube_location_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_coin_cube_location ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_coin_cube_location_index_seq
    START WITH 1599
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 260 (class 1259 OID 23912)
-- Name: pangya_comet_refill; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_comet_refill (
    index bigint NOT NULL,
    typeid integer NOT NULL,
    min smallint NOT NULL,
    max smallint NOT NULL
);


ALTER TABLE pangya.pangya_comet_refill OWNER TO postgres;

--
-- TOC entry 261 (class 1259 OID 23915)
-- Name: pangya_comet_refill_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_comet_refill ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_comet_refill_index_seq
    START WITH 2
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 262 (class 1259 OID 23917)
-- Name: pangya_command; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_command (
    idx bigint NOT NULL,
    command_id integer DEFAULT 0 NOT NULL,
    arg1 integer DEFAULT 0 NOT NULL,
    arg2 integer DEFAULT 0 NOT NULL,
    arg3 integer DEFAULT 0 NOT NULL,
    arg4 integer DEFAULT 0 NOT NULL,
    arg5 integer DEFAULT 0 NOT NULL,
    target integer DEFAULT 0 NOT NULL,
    regdate timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL,
    reservedate timestamp without time zone,
    flag smallint DEFAULT 0 NOT NULL,
    valid smallint DEFAULT 1 NOT NULL
);


ALTER TABLE pangya.pangya_command OWNER TO postgres;

--
-- TOC entry 263 (class 1259 OID 23930)
-- Name: pangya_command_idx_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_command ALTER COLUMN idx ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_command_idx_seq
    START WITH 6176
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 264 (class 1259 OID 23932)
-- Name: pangya_config; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_config (
    uid integer NOT NULL,
    grandzodiaceventtime smallint DEFAULT 0 NOT NULL,
    scratchyporpointrate smallint DEFAULT 100 NOT NULL,
    papelshoprareitemrate smallint DEFAULT 100 NOT NULL,
    papelshopcookieitemrate smallint DEFAULT 100 NOT NULL,
    treasurerate smallint DEFAULT 100 NOT NULL,
    pangrate smallint DEFAULT 100 NOT NULL,
    exprate smallint DEFAULT 100 NOT NULL,
    clubmasteryrate smallint DEFAULT 100 NOT NULL,
    chuvarate smallint DEFAULT 100 NOT NULL,
    memorialshoprate smallint DEFAULT 100 NOT NULL,
    angelevent smallint DEFAULT 0 NOT NULL,
    grandprixevent smallint DEFAULT 0 NOT NULL,
    goldentimeevent smallint DEFAULT 0 NOT NULL,
    loginrewardevent smallint DEFAULT 0 NOT NULL,
    botgmevent smallint DEFAULT 0 NOT NULL,
    smartcalculator smallint DEFAULT 0 NOT NULL
);


ALTER TABLE pangya.pangya_config OWNER TO postgres;

--
-- TOC entry 265 (class 1259 OID 23951)
-- Name: pangya_cookie_point_item_log; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_cookie_point_item_log (
    index bigint NOT NULL,
    cp_id_log bigint DEFAULT 0,
    typeid integer DEFAULT 0,
    qnty integer DEFAULT 0,
    price bigint DEFAULT 0
);


ALTER TABLE pangya.pangya_cookie_point_item_log OWNER TO postgres;

--
-- TOC entry 266 (class 1259 OID 23958)
-- Name: pangya_cookie_point_item_log_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_cookie_point_item_log ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_cookie_point_item_log_index_seq
    START WITH 542
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 267 (class 1259 OID 23960)
-- Name: pangya_cookie_point_log; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_cookie_point_log (
    id bigint NOT NULL,
    uid integer DEFAULT 0,
    type smallint DEFAULT 0,
    mail_id integer DEFAULT '-1'::integer,
    cookie bigint DEFAULT 0,
    item_qnty integer DEFAULT 0,
    reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_cookie_point_log OWNER TO postgres;

--
-- TOC entry 268 (class 1259 OID 23969)
-- Name: pangya_cookie_point_log_id_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_cookie_point_log ALTER COLUMN id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_cookie_point_log_id_seq
    START WITH 634
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 269 (class 1259 OID 23971)
-- Name: pangya_counter_item; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_counter_item (
    count_id integer NOT NULL,
    uid integer NOT NULL,
    typeid integer NOT NULL,
    active integer DEFAULT 1 NOT NULL,
    count_num_item integer DEFAULT 0 NOT NULL
);


ALTER TABLE pangya.pangya_counter_item OWNER TO postgres;

--
-- TOC entry 270 (class 1259 OID 23976)
-- Name: pangya_counter_item_count_id_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_counter_item ALTER COLUMN count_id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_counter_item_count_id_seq
    START WITH 85913
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 271 (class 1259 OID 23978)
-- Name: pangya_coupon_desconto; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_coupon_desconto (
    nome character varying(50) NOT NULL,
    typeid integer NOT NULL,
    valor integer NOT NULL
);


ALTER TABLE pangya.pangya_coupon_desconto OWNER TO postgres;

--
-- TOC entry 272 (class 1259 OID 23981)
-- Name: pangya_course_cube_coin_temporada; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_course_cube_coin_temporada (
    index integer NOT NULL,
    course integer DEFAULT 0 NOT NULL,
    active smallint DEFAULT 1 NOT NULL
);


ALTER TABLE pangya.pangya_course_cube_coin_temporada OWNER TO postgres;

--
-- TOC entry 273 (class 1259 OID 23986)
-- Name: pangya_course_cube_coin_temporada_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_course_cube_coin_temporada ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_course_cube_coin_temporada_index_seq
    START WITH 22
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 274 (class 1259 OID 23988)
-- Name: pangya_course_drop; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_course_drop (
    index bigint NOT NULL,
    course integer DEFAULT 0 NOT NULL,
    rate integer DEFAULT 25 NOT NULL,
    active smallint DEFAULT 1 NOT NULL
);


ALTER TABLE pangya.pangya_course_drop OWNER TO postgres;

--
-- TOC entry 275 (class 1259 OID 23994)
-- Name: pangya_course_drop_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_course_drop ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_course_drop_index_seq
    START WITH 2
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 276 (class 1259 OID 23996)
-- Name: pangya_course_drop_item; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_course_drop_item (
    index bigint NOT NULL,
    course integer DEFAULT 0 NOT NULL,
    drop_typeid integer DEFAULT 0 NOT NULL,
    active smallint DEFAULT 1 NOT NULL
);


ALTER TABLE pangya.pangya_course_drop_item OWNER TO postgres;

--
-- TOC entry 277 (class 1259 OID 24002)
-- Name: pangya_course_drop_item_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_course_drop_item ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_course_drop_item_index_seq
    START WITH 2
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 278 (class 1259 OID 24004)
-- Name: pangya_course_reward_treasure; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_course_reward_treasure (
    course smallint NOT NULL,
    pangreward integer NOT NULL
);


ALTER TABLE pangya.pangya_course_reward_treasure OWNER TO postgres;

--
-- TOC entry 279 (class 1259 OID 24007)
-- Name: pangya_cube_coin_location; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_cube_coin_location (
    index bigint NOT NULL,
    tipo integer NOT NULL,
    config2 integer NOT NULL,
    course smallint NOT NULL,
    hole smallint NOT NULL,
    x real NOT NULL,
    y real NOT NULL,
    z real NOT NULL
);


ALTER TABLE pangya.pangya_cube_coin_location OWNER TO postgres;

--
-- TOC entry 280 (class 1259 OID 24010)
-- Name: pangya_cube_coin_location_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_cube_coin_location ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_cube_coin_location_index_seq
    START WITH 356
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 281 (class 1259 OID 24012)
-- Name: pangya_daily_quest; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_daily_quest (
    achieve_quest_1 integer DEFAULT 0 NOT NULL,
    achieve_quest_2 integer DEFAULT 0 NOT NULL,
    achieve_quest_3 integer DEFAULT 0 NOT NULL,
    reg_date timestamp without time zone
);


ALTER TABLE pangya.pangya_daily_quest OWNER TO postgres;

--
-- TOC entry 282 (class 1259 OID 24018)
-- Name: pangya_daily_quest_player; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_daily_quest_player (
    uid bigint NOT NULL,
    last_quest_accept timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    today_quest timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


ALTER TABLE pangya.pangya_daily_quest_player OWNER TO postgres;

--
-- TOC entry 283 (class 1259 OID 24023)
-- Name: pangya_dolfini_locker; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_dolfini_locker (
    uid integer NOT NULL,
    senha character varying(5) DEFAULT NULL::character varying,
    pang bigint DEFAULT 0 NOT NULL,
    locker smallint DEFAULT 0 NOT NULL
);


ALTER TABLE pangya.pangya_dolfini_locker OWNER TO postgres;

--
-- TOC entry 284 (class 1259 OID 24029)
-- Name: pangya_dolfini_locker_item; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_dolfini_locker_item (
    idx bigint NOT NULL,
    uid integer NOT NULL,
    item_id integer NOT NULL,
    flag smallint DEFAULT 1 NOT NULL
);


ALTER TABLE pangya.pangya_dolfini_locker_item OWNER TO postgres;

--
-- TOC entry 285 (class 1259 OID 24033)
-- Name: pangya_dolfini_locker_item_idx_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_dolfini_locker_item ALTER COLUMN idx ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_dolfini_locker_item_idx_seq
    START WITH 172
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 286 (class 1259 OID 24035)
-- Name: pangya_donation_epin; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_donation_epin (
    index bigint NOT NULL,
    donation_id bigint NOT NULL,
    uid integer NOT NULL,
    epin uuid DEFAULT gen_random_uuid() NOT NULL,
    qntd bigint NOT NULL,
    retrive_uid integer,
    valid smallint DEFAULT 1 NOT NULL
);


ALTER TABLE pangya.pangya_donation_epin OWNER TO postgres;

--
-- TOC entry 287 (class 1259 OID 24040)
-- Name: pangya_donation_epin_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_donation_epin ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_donation_epin_index_seq
    START WITH 54
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 288 (class 1259 OID 24042)
-- Name: pangya_donation_item_log; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_donation_item_log (
    index bigint NOT NULL,
    donation_id bigint DEFAULT 0 NOT NULL,
    item_typeid integer DEFAULT 0 NOT NULL,
    item_qntd integer DEFAULT 0 NOT NULL
);


ALTER TABLE pangya.pangya_donation_item_log OWNER TO postgres;

--
-- TOC entry 289 (class 1259 OID 24048)
-- Name: pangya_donation_item_log_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_donation_item_log ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_donation_item_log_index_seq
    START WITH 58
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 290 (class 1259 OID 24050)
-- Name: pangya_donation_log; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_donation_log (
    index bigint NOT NULL,
    adm_uid integer DEFAULT 0 NOT NULL,
    uid integer DEFAULT 0 NOT NULL,
    plataforma smallint DEFAULT 0 NOT NULL,
    cash integer DEFAULT 0 NOT NULL,
    cookie_point integer DEFAULT 0 NOT NULL,
    email character varying(100),
    obs character varying(500),
    red_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_donation_log OWNER TO postgres;

--
-- TOC entry 291 (class 1259 OID 24062)
-- Name: pangya_donation_log_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_donation_log ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_donation_log_index_seq
    START WITH 22
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 292 (class 1259 OID 24064)
-- Name: pangya_donation_new; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_donation_new (
    index bigint NOT NULL,
    uid integer DEFAULT '-1'::integer NOT NULL,
    plataforma smallint NOT NULL,
    email character varying(200) NOT NULL,
    date timestamp without time zone NOT NULL,
    update timestamp without time zone,
    code character varying(50) NOT NULL,
    type smallint NOT NULL,
    status smallint NOT NULL,
    reference character varying(200) NOT NULL,
    gross_amount double precision NOT NULL,
    net_amount double precision NOT NULL,
    escrow timestamp without time zone,
    epin_id bigint DEFAULT '-1'::integer NOT NULL,
    reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_donation_new OWNER TO postgres;

--
-- TOC entry 293 (class 1259 OID 24070)
-- Name: pangya_donation_new_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_donation_new ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_donation_new_index_seq
    START WITH 51
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 294 (class 1259 OID 24072)
-- Name: pangya_friend_list; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_friend_list (
    uid integer DEFAULT 0 NOT NULL,
    uid_friend integer DEFAULT 0 NOT NULL,
    apelido character varying(15) DEFAULT 'Friend'::character varying NOT NULL,
    unknown1 integer DEFAULT '-1'::integer NOT NULL,
    unknown2 integer DEFAULT 0 NOT NULL,
    unknown3 integer DEFAULT '-1'::integer NOT NULL,
    unknown4 integer DEFAULT 0 NOT NULL,
    unknown5 integer DEFAULT 0 NOT NULL,
    unknown6 integer DEFAULT 0 NOT NULL,
    flag1 smallint DEFAULT '-1'::integer NOT NULL,
    state_flag smallint DEFAULT 0 NOT NULL,
    flag5 smallint DEFAULT 0 NOT NULL
);


ALTER TABLE pangya.pangya_friend_list OWNER TO postgres;

--
-- TOC entry 295 (class 1259 OID 24087)
-- Name: pangya_gacha_jp_all_item_list; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_gacha_jp_all_item_list (
    index bigint NOT NULL,
    typeid integer NOT NULL,
    name character varying(100) NOT NULL,
    char_type smallint DEFAULT 0 NOT NULL,
    reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_gacha_jp_all_item_list OWNER TO postgres;

--
-- TOC entry 296 (class 1259 OID 24092)
-- Name: pangya_gacha_jp_all_item_list_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_gacha_jp_all_item_list ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_gacha_jp_all_item_list_index_seq
    START WITH 74
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 297 (class 1259 OID 24094)
-- Name: pangya_gacha_jp_item_list; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_gacha_jp_item_list (
    index bigint NOT NULL,
    active smallint DEFAULT 1 NOT NULL,
    gacha_num integer NOT NULL,
    typeid_1 integer NOT NULL,
    typeid_2 integer,
    qnty_1 bigint NOT NULL,
    qnty_2 bigint,
    rarity_type smallint NOT NULL,
    reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_gacha_jp_item_list OWNER TO postgres;

--
-- TOC entry 298 (class 1259 OID 24099)
-- Name: pangya_gacha_jp_item_list_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_gacha_jp_item_list ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_gacha_jp_item_list_index_seq
    START WITH 91
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 299 (class 1259 OID 24101)
-- Name: pangya_gacha_jp_player_win; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_gacha_jp_player_win (
    index bigint NOT NULL,
    uid integer NOT NULL,
    gacha_num integer NOT NULL,
    typeid integer NOT NULL,
    qnty bigint NOT NULL,
    rarity_type smallint NOT NULL,
    send_mail smallint NOT NULL,
    valid smallint NOT NULL,
    reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_gacha_jp_player_win OWNER TO postgres;

--
-- TOC entry 300 (class 1259 OID 24105)
-- Name: pangya_gacha_jp_player_win_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_gacha_jp_player_win ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_gacha_jp_player_win_index_seq
    START WITH 2405
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 301 (class 1259 OID 24107)
-- Name: pangya_gacha_jp_rate; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_gacha_jp_rate (
    gacha_num integer NOT NULL,
    rate_rare integer DEFAULT 100 NOT NULL,
    rate_normal integer DEFAULT 100 NOT NULL
);


ALTER TABLE pangya.pangya_gacha_jp_rate OWNER TO postgres;

--
-- TOC entry 302 (class 1259 OID 24112)
-- Name: pangya_gift_table; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_gift_table (
    uid integer NOT NULL,
    msg_id bigint NOT NULL,
    flag smallint DEFAULT 0 NOT NULL,
    fromid character varying(30) NOT NULL,
    message character varying(500) DEFAULT ''::character varying NOT NULL,
    giftdate timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL,
    enddate timestamp without time zone,
    contador_vista integer DEFAULT 0 NOT NULL,
    lida_yn smallint DEFAULT 0 NOT NULL,
    valid smallint DEFAULT 1 NOT NULL
);


ALTER TABLE pangya.pangya_gift_table OWNER TO postgres;

--
-- TOC entry 303 (class 1259 OID 24124)
-- Name: pangya_gift_table_msg_id_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_gift_table ALTER COLUMN msg_id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_gift_table_msg_id_seq
    START WITH 10045
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 304 (class 1259 OID 24126)
-- Name: pangya_gm_gift_web_log; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_gm_gift_web_log (
    index bigint NOT NULL,
    gm_uid integer NOT NULL,
    player_uid integer NOT NULL,
    msg_id integer NOT NULL,
    reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_gm_gift_web_log OWNER TO postgres;

--
-- TOC entry 305 (class 1259 OID 24130)
-- Name: pangya_gm_gift_web_log_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_gm_gift_web_log ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_gm_gift_web_log_index_seq
    START WITH 37
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 306 (class 1259 OID 24132)
-- Name: pangya_golden_time_info; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_golden_time_info (
    index integer NOT NULL,
    type smallint DEFAULT 0 NOT NULL,
    begin date NOT NULL,
    "end" date,
    rate integer DEFAULT 1 NOT NULL,
    is_end smallint DEFAULT 0 NOT NULL,
    reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_golden_time_info OWNER TO postgres;

--
-- TOC entry 307 (class 1259 OID 24139)
-- Name: pangya_golden_time_info_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_golden_time_info ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_golden_time_info_index_seq
    START WITH 10
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 308 (class 1259 OID 24141)
-- Name: pangya_golden_time_item; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_golden_time_item (
    index bigint NOT NULL,
    golden_time_id bigint NOT NULL,
    typeid integer NOT NULL,
    qntd integer NOT NULL,
    qntd_time integer NOT NULL,
    rate integer DEFAULT 100 NOT NULL,
    reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_golden_time_item OWNER TO postgres;

--
-- TOC entry 309 (class 1259 OID 24146)
-- Name: pangya_golden_time_item_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_golden_time_item ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_golden_time_item_index_seq
    START WITH 34
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 310 (class 1259 OID 24148)
-- Name: pangya_golden_time_round; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_golden_time_round (
    index bigint NOT NULL,
    golden_time_id bigint NOT NULL,
    "time" time(6) without time zone NOT NULL,
    reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_golden_time_round OWNER TO postgres;

--
-- TOC entry 311 (class 1259 OID 24152)
-- Name: pangya_golden_time_round_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_golden_time_round ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_golden_time_round_index_seq
    START WITH 22
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 312 (class 1259 OID 24154)
-- Name: pangya_grand_zodiac_pontos; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_grand_zodiac_pontos (
    uid integer NOT NULL,
    pontos bigint DEFAULT 0 NOT NULL
);


ALTER TABLE pangya.pangya_grand_zodiac_pontos OWNER TO postgres;

--
-- TOC entry 313 (class 1259 OID 24158)
-- Name: pangya_grand_zodiac_times; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_grand_zodiac_times (
    index bigint NOT NULL,
    inicio_time time(6) without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL,
    fim_time time(6) without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL,
    type smallint DEFAULT 0 NOT NULL,
    valid smallint DEFAULT 1 NOT NULL
);


ALTER TABLE pangya.pangya_grand_zodiac_times OWNER TO postgres;

--
-- TOC entry 314 (class 1259 OID 24165)
-- Name: pangya_grand_zodiac_times_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_grand_zodiac_times ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_grand_zodiac_times_index_seq
    START WITH 18
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 315 (class 1259 OID 24167)
-- Name: pangya_grandprix_clear; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_grandprix_clear (
    index bigint NOT NULL,
    uid integer DEFAULT 0 NOT NULL,
    typeid integer DEFAULT 0 NOT NULL,
    flag integer DEFAULT 0 NOT NULL
);


ALTER TABLE pangya.pangya_grandprix_clear OWNER TO postgres;

--
-- TOC entry 316 (class 1259 OID 24173)
-- Name: pangya_grandprix_clear_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_grandprix_clear ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_grandprix_clear_index_seq
    START WITH 86
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 317 (class 1259 OID 24175)
-- Name: pangya_grandprix_event_config; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_grandprix_event_config (
    index bigint NOT NULL,
    flag integer DEFAULT 0 NOT NULL,
    active smallint DEFAULT 1 NOT NULL
);


ALTER TABLE pangya.pangya_grandprix_event_config OWNER TO postgres;

--
-- TOC entry 318 (class 1259 OID 24180)
-- Name: pangya_grandprix_event_config_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_grandprix_event_config ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_grandprix_event_config_index_seq
    START WITH 5
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 319 (class 1259 OID 24182)
-- Name: pangya_guild; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_guild (
    guild_uid bigint NOT NULL,
    guild_id character varying(8) DEFAULT ''::character varying NOT NULL,
    guild_name character varying(50) NOT NULL,
    guild_leader integer NOT NULL,
    guild_sub_master integer DEFAULT 0 NOT NULL,
    guild_condition_level smallint DEFAULT 0 NOT NULL,
    guild_state smallint DEFAULT 0 NOT NULL,
    guild_flag smallint DEFAULT 0 NOT NULL,
    guild_permition_join smallint DEFAULT 1 NOT NULL,
    guild_pang bigint DEFAULT 0 NOT NULL,
    guild_point bigint DEFAULT 0 NOT NULL,
    guild_win integer DEFAULT 0 NOT NULL,
    guild_lose integer DEFAULT 0 NOT NULL,
    guild_draw integer DEFAULT 0 NOT NULL,
    guild_mark_img character varying(12) DEFAULT 'guildmark'::character varying NOT NULL,
    guild_mark_img_idx integer DEFAULT 0 NOT NULL,
    guild_new_mark_idx integer DEFAULT 0 NOT NULL,
    guild_intro_img character varying(50),
    guild_notice character varying(110) DEFAULT ''::character varying NOT NULL,
    guild_info character varying(110) DEFAULT ''::character varying NOT NULL,
    guild_reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL,
    guild_accept_date timestamp without time zone,
    guild_closure_date timestamp without time zone
);


ALTER TABLE pangya.pangya_guild OWNER TO postgres;

--
-- TOC entry 320 (class 1259 OID 24202)
-- Name: pangya_guild_atividade_player; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_guild_atividade_player (
    idx bigint NOT NULL,
    uid integer NOT NULL,
    guild_uid integer NOT NULL,
    flag integer NOT NULL,
    reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_guild_atividade_player OWNER TO postgres;

--
-- TOC entry 321 (class 1259 OID 24206)
-- Name: pangya_guild_atividade_player_idx_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_guild_atividade_player ALTER COLUMN idx ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_guild_atividade_player_idx_seq
    START WITH 426
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 322 (class 1259 OID 24208)
-- Name: pangya_guild_bbs; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_guild_bbs (
    seq bigint NOT NULL,
    owner_uid integer NOT NULL,
    title character varying(50) NOT NULL,
    text character varying(500) NOT NULL,
    type smallint DEFAULT 0 NOT NULL,
    state smallint DEFAULT 1 NOT NULL,
    views bigint DEFAULT 0 NOT NULL,
    reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_guild_bbs OWNER TO postgres;

--
-- TOC entry 323 (class 1259 OID 24218)
-- Name: pangya_guild_bbs_res; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_guild_bbs_res (
    seq bigint NOT NULL,
    bbs_seq bigint NOT NULL,
    owner_uid integer NOT NULL,
    text character varying(500) NOT NULL,
    state smallint DEFAULT 1 NOT NULL,
    reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_guild_bbs_res OWNER TO postgres;

--
-- TOC entry 324 (class 1259 OID 24226)
-- Name: pangya_guild_bbs_res_seq_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_guild_bbs_res ALTER COLUMN seq ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_guild_bbs_res_seq_seq
    START WITH 32
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 325 (class 1259 OID 24228)
-- Name: pangya_guild_bbs_seq_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_guild_bbs ALTER COLUMN seq ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_guild_bbs_seq_seq
    START WITH 48
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 326 (class 1259 OID 24230)
-- Name: pangya_guild_guild_uid_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_guild ALTER COLUMN guild_uid ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_guild_guild_uid_seq
    START WITH 19
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 327 (class 1259 OID 24232)
-- Name: pangya_guild_intro_img_log; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_guild_intro_img_log (
    index bigint NOT NULL,
    intro_img character varying(50) NOT NULL,
    reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_guild_intro_img_log OWNER TO postgres;

--
-- TOC entry 328 (class 1259 OID 24236)
-- Name: pangya_guild_intro_img_log_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_guild_intro_img_log ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_guild_intro_img_log_index_seq
    START WITH 11
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 329 (class 1259 OID 24238)
-- Name: pangya_guild_mark_log; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_guild_mark_log (
    index bigint NOT NULL,
    mark_idx integer NOT NULL,
    reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_guild_mark_log OWNER TO postgres;

--
-- TOC entry 330 (class 1259 OID 24242)
-- Name: pangya_guild_mark_log_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_guild_mark_log ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_guild_mark_log_index_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 331 (class 1259 OID 24244)
-- Name: pangya_guild_match; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_guild_match (
    index bigint NOT NULL,
    guild_1_uid integer NOT NULL,
    guild_2_uid integer NOT NULL,
    guild_1_point integer NOT NULL,
    guild_2_point integer NOT NULL,
    guild_1_pang integer NOT NULL,
    guild_2_pang integer NOT NULL,
    reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_guild_match OWNER TO postgres;

--
-- TOC entry 332 (class 1259 OID 24248)
-- Name: pangya_guild_match_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_guild_match ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_guild_match_index_seq
    START WITH 10
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 333 (class 1259 OID 24250)
-- Name: pangya_guild_member; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_guild_member (
    guild_uid integer DEFAULT 0 NOT NULL,
    member_uid integer DEFAULT 0 NOT NULL,
    member_msg character varying(64) DEFAULT NULL::character varying,
    guild_pang integer DEFAULT 0 NOT NULL,
    guild_point integer DEFAULT 0 NOT NULL,
    member_flag integer DEFAULT 0 NOT NULL,
    member_state_flag integer DEFAULT 3 NOT NULL,
    reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_guild_member OWNER TO postgres;

--
-- TOC entry 334 (class 1259 OID 24261)
-- Name: pangya_guild_notice; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_guild_notice (
    seq bigint NOT NULL,
    guild_uid integer NOT NULL,
    owner_uid integer NOT NULL,
    title character varying(50) NOT NULL,
    text character varying(500) NOT NULL,
    state smallint DEFAULT 1 NOT NULL,
    views bigint DEFAULT 0 NOT NULL,
    reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_guild_notice OWNER TO postgres;

--
-- TOC entry 335 (class 1259 OID 24270)
-- Name: pangya_guild_notice_seq_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_guild_notice ALTER COLUMN seq ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_guild_notice_seq_seq
    START WITH 60
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 336 (class 1259 OID 24272)
-- Name: pangya_guild_private_bbs; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_guild_private_bbs (
    seq bigint NOT NULL,
    guild_uid integer NOT NULL,
    owner_uid integer NOT NULL,
    title character varying(50) NOT NULL,
    text character varying(500) NOT NULL,
    views bigint DEFAULT 0 NOT NULL,
    state smallint DEFAULT 1 NOT NULL,
    reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_guild_private_bbs OWNER TO postgres;

--
-- TOC entry 337 (class 1259 OID 24281)
-- Name: pangya_guild_private_bbs_res; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_guild_private_bbs_res (
    seq bigint NOT NULL,
    guild_bbs_seq bigint NOT NULL,
    owner_uid integer NOT NULL,
    text character varying(500) NOT NULL,
    state smallint DEFAULT 1 NOT NULL,
    reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


ALTER TABLE pangya.pangya_guild_private_bbs_res OWNER TO postgres;

--
-- TOC entry 338 (class 1259 OID 24289)
-- Name: pangya_guild_private_bbs_res_seq_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_guild_private_bbs_res ALTER COLUMN seq ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_guild_private_bbs_res_seq_seq
    START WITH 52
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 339 (class 1259 OID 24291)
-- Name: pangya_guild_private_bbs_seq_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_guild_private_bbs ALTER COLUMN seq ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_guild_private_bbs_seq_seq
    START WITH 68
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 340 (class 1259 OID 24293)
-- Name: pangya_guild_ranking; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_guild_ranking (
    index bigint NOT NULL,
    guild_uid integer DEFAULT 0 NOT NULL,
    rank integer DEFAULT 1 NOT NULL,
    last_rank integer NOT NULL,
    reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_guild_ranking OWNER TO postgres;

--
-- TOC entry 341 (class 1259 OID 24299)
-- Name: pangya_guild_ranking_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_guild_ranking ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_guild_ranking_index_seq
    START WITH 4
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 342 (class 1259 OID 24301)
-- Name: pangya_guild_update_activity; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_guild_update_activity (
    index bigint NOT NULL,
    guild_uid integer NOT NULL,
    owner_update integer NOT NULL,
    player_uid integer NOT NULL,
    type_update smallint DEFAULT 0 NOT NULL,
    state smallint DEFAULT 0 NOT NULL,
    reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_guild_update_activity OWNER TO postgres;

--
-- TOC entry 343 (class 1259 OID 24307)
-- Name: pangya_guild_update_activity_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_guild_update_activity ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_guild_update_activity_index_seq
    START WITH 58
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 344 (class 1259 OID 24309)
-- Name: pangya_gz_event_2016121600_rare_win; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_gz_event_2016121600_rare_win (
    index bigint NOT NULL,
    uid integer DEFAULT 0 NOT NULL,
    item_typeid integer DEFAULT 0,
    win_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


ALTER TABLE pangya.pangya_gz_event_2016121600_rare_win OWNER TO postgres;

--
-- TOC entry 345 (class 1259 OID 24315)
-- Name: pangya_gz_event_2016121600_rare_win_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_gz_event_2016121600_rare_win ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_gz_event_2016121600_rare_win_index_seq
    START WITH 58
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 346 (class 1259 OID 24317)
-- Name: pangya_ip_table; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_ip_table (
    index bigint NOT NULL,
    ip character varying(40) NOT NULL,
    mask character varying(18) DEFAULT '255.255.255.255'::character varying NOT NULL,
    date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_ip_table OWNER TO postgres;

--
-- TOC entry 347 (class 1259 OID 24322)
-- Name: pangya_ip_table_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_ip_table ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_ip_table_index_seq
    START WITH 3
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 348 (class 1259 OID 24324)
-- Name: pangya_item_buff; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_item_buff (
    index bigint NOT NULL,
    uid integer NOT NULL,
    typeid integer NOT NULL,
    reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL,
    end_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL,
    tipo smallint DEFAULT 2 NOT NULL,
    percent integer DEFAULT 0 NOT NULL,
    use_yn smallint DEFAULT 1 NOT NULL
);


ALTER TABLE pangya.pangya_item_buff OWNER TO postgres;

--
-- TOC entry 349 (class 1259 OID 24332)
-- Name: pangya_item_buff_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_item_buff ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_item_buff_index_seq
    START WITH 21
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 350 (class 1259 OID 24334)
-- Name: pangya_item_mail; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_item_mail (
    msg_id integer NOT NULL,
    item_id integer DEFAULT '-1'::integer NOT NULL,
    item_typeid integer NOT NULL,
    flag smallint DEFAULT 0 NOT NULL,
    get_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    quantidade_item integer DEFAULT 0 NOT NULL,
    quantidade_dia integer DEFAULT 0 NOT NULL,
    pang bigint DEFAULT 0 NOT NULL,
    cookie bigint DEFAULT 0 NOT NULL,
    gm_id integer DEFAULT '-1'::integer NOT NULL,
    flag_gift integer DEFAULT 0 NOT NULL,
    ucc_img_mark character varying(12) DEFAULT '0'::character varying NOT NULL,
    type smallint DEFAULT 0 NOT NULL,
    valid smallint DEFAULT 1 NOT NULL
);


ALTER TABLE pangya.pangya_item_mail OWNER TO postgres;

--
-- TOC entry 351 (class 1259 OID 24349)
-- Name: pangya_item_warehouse; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_item_warehouse (
    item_id bigint NOT NULL,
    uid integer NOT NULL,
    typeid integer NOT NULL,
    valid smallint DEFAULT 1 NOT NULL,
    regdate timestamp without time zone,
    gift_flag smallint DEFAULT 0 NOT NULL,
    flag smallint DEFAULT 0 NOT NULL,
    applytime timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    enddate timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    c0 smallint DEFAULT 0 NOT NULL,
    c1 smallint DEFAULT 0 NOT NULL,
    c2 smallint DEFAULT 0 NOT NULL,
    c3 smallint DEFAULT 0 NOT NULL,
    c4 smallint DEFAULT 0 NOT NULL,
    purchase smallint DEFAULT 0 NOT NULL,
    itemtype smallint DEFAULT 2 NOT NULL,
    clubset_workshop_flag smallint DEFAULT 0 NOT NULL,
    clubset_workshop_c0 smallint DEFAULT 0 NOT NULL,
    clubset_workshop_c1 smallint DEFAULT 0 NOT NULL,
    clubset_workshop_c2 smallint DEFAULT 0 NOT NULL,
    clubset_workshop_c3 smallint DEFAULT 0 NOT NULL,
    clubset_workshop_c4 smallint DEFAULT 0 NOT NULL,
    mastery_pts integer DEFAULT 0 NOT NULL,
    recovery_pts integer DEFAULT 0 NOT NULL,
    level integer DEFAULT 0 NOT NULL,
    up integer DEFAULT 0 NOT NULL,
    total_mastery_pts bigint DEFAULT 0 NOT NULL,
    mastery_gasto integer DEFAULT 0 NOT NULL
);


ALTER TABLE pangya.pangya_item_warehouse OWNER TO postgres;

--
-- TOC entry 352 (class 1259 OID 24376)
-- Name: pangya_item_warehouse_item_id_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_item_warehouse ALTER COLUMN item_id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_item_warehouse_item_id_seq
    START WITH 11343
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 353 (class 1259 OID 24378)
-- Name: pangya_last_players_user; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_last_players_user (
    uid integer NOT NULL,
    sex_0 integer,
    id_0 character varying(22) DEFAULT NULL::character varying,
    nick_0 character varying(22) DEFAULT NULL::character varying,
    uid_0 integer,
    sex_1 integer,
    id_1 character varying(22) DEFAULT NULL::character varying,
    nick_1 character varying(22) DEFAULT NULL::character varying,
    uid_1 integer,
    sex_2 integer,
    id_2 character varying(22) DEFAULT NULL::character varying,
    nick_2 character varying(22) DEFAULT NULL::character varying,
    uid_2 integer,
    sex_3 integer,
    id_3 character varying(22) DEFAULT NULL::character varying,
    nick_3 character varying(22) DEFAULT NULL::character varying,
    uid_3 integer,
    sex_4 integer,
    id_4 character varying(22) DEFAULT NULL::character varying,
    nick_4 character varying(22) DEFAULT NULL::character varying,
    uid_4 integer
);


ALTER TABLE pangya.pangya_last_players_user OWNER TO postgres;

--
-- TOC entry 354 (class 1259 OID 24391)
-- Name: pangya_last_up_clubset; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_last_up_clubset (
    item_id integer NOT NULL,
    uid integer NOT NULL,
    state smallint NOT NULL,
    item_usado integer NOT NULL,
    quantidade integer NOT NULL
);


ALTER TABLE pangya.pangya_last_up_clubset OWNER TO postgres;

--
-- TOC entry 355 (class 1259 OID 24394)
-- Name: pangya_login_reward; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_login_reward (
    index bigint NOT NULL,
    name character varying(50) NOT NULL,
    type smallint DEFAULT 0 NOT NULL,
    days_to_gift integer DEFAULT 1 NOT NULL,
    n_times_gift integer DEFAULT 0 NOT NULL,
    item_typeid integer NOT NULL,
    item_qntd integer NOT NULL,
    item_qntd_time integer NOT NULL,
    is_end smallint DEFAULT 0 NOT NULL,
    end_date timestamp without time zone,
    reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_login_reward OWNER TO postgres;

--
-- TOC entry 356 (class 1259 OID 24402)
-- Name: pangya_login_reward_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_login_reward ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_login_reward_index_seq
    START WITH 12
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 357 (class 1259 OID 24404)
-- Name: pangya_login_reward_player; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_login_reward_player (
    index bigint NOT NULL,
    login_reward_id bigint NOT NULL,
    uid integer NOT NULL,
    count_days integer DEFAULT 1 NOT NULL,
    count_seq integer DEFAULT 0 NOT NULL,
    is_clear smallint DEFAULT 0 NOT NULL,
    update_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL,
    reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_login_reward_player OWNER TO postgres;

--
-- TOC entry 358 (class 1259 OID 24412)
-- Name: pangya_login_reward_player_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_login_reward_player ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_login_reward_player_index_seq
    START WITH 159
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 359 (class 1259 OID 24414)
-- Name: pangya_lucia_attendance; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_lucia_attendance (
    uid integer NOT NULL,
    count_day bigint DEFAULT 0 NOT NULL,
    last_day_attendance timestamp without time zone,
    last_day_get_item timestamp without time zone,
    try_hacking_count integer DEFAULT 0 NOT NULL,
    block_type smallint DEFAULT 0 NOT NULL,
    block_end_date timestamp without time zone
);


ALTER TABLE pangya.pangya_lucia_attendance OWNER TO postgres;

--
-- TOC entry 360 (class 1259 OID 24420)
-- Name: pangya_lucia_attendance_reward_log; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_lucia_attendance_reward_log (
    index bigint NOT NULL,
    uid integer NOT NULL,
    msg_id integer NOT NULL,
    error character varying(50),
    reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_lucia_attendance_reward_log OWNER TO postgres;

--
-- TOC entry 361 (class 1259 OID 24424)
-- Name: pangya_lucia_attendance_reward_log_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_lucia_attendance_reward_log ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_lucia_attendance_reward_log_index_seq
    START WITH 107
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 362 (class 1259 OID 24426)
-- Name: pangya_mac_table; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_mac_table (
    index bigint NOT NULL,
    mac character varying(20) NOT NULL,
    date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_mac_table OWNER TO postgres;

--
-- TOC entry 363 (class 1259 OID 24430)
-- Name: pangya_mac_table_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_mac_table ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_mac_table_index_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 364 (class 1259 OID 24432)
-- Name: pangya_mascot_info; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_mascot_info (
    item_id bigint NOT NULL,
    uid integer NOT NULL,
    typeid integer NOT NULL,
    mlevel smallint DEFAULT 0 NOT NULL,
    mexp integer DEFAULT 0 NOT NULL,
    flag smallint DEFAULT 0 NOT NULL,
    tipo smallint DEFAULT 0 NOT NULL,
    regdate timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL,
    period smallint DEFAULT 0 NOT NULL,
    enddate timestamp without time zone,
    message character varying(30) DEFAULT 'Pangya SuperSS'::character varying NOT NULL,
    iscash smallint DEFAULT 0 NOT NULL,
    price integer DEFAULT 0 NOT NULL,
    valid smallint DEFAULT 1 NOT NULL
);


ALTER TABLE pangya.pangya_mascot_info OWNER TO postgres;

--
-- TOC entry 365 (class 1259 OID 24445)
-- Name: pangya_mascot_info_item_id_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_mascot_info ALTER COLUMN item_id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_mascot_info_item_id_seq
    START WITH 182
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 366 (class 1259 OID 24447)
-- Name: pangya_memorial_shop_coin_item; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_memorial_shop_coin_item (
    typeid integer NOT NULL,
    tipo integer NOT NULL,
    valor_1 integer NOT NULL,
    valor_2 integer NOT NULL,
    character_type integer NOT NULL,
    outro_item integer NOT NULL,
    tipo_item integer NOT NULL
);


ALTER TABLE pangya.pangya_memorial_shop_coin_item OWNER TO postgres;

--
-- TOC entry 367 (class 1259 OID 24450)
-- Name: pangya_memorial_shop_level; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_memorial_shop_level (
    level integer NOT NULL,
    gacha_inicio integer NOT NULL,
    gacha_fim integer NOT NULL
);


ALTER TABLE pangya.pangya_memorial_shop_level OWNER TO postgres;

--
-- TOC entry 368 (class 1259 OID 24453)
-- Name: pangya_memorial_shop_luckyset; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_memorial_shop_luckyset (
    nome character varying(50) DEFAULT NULL::character varying,
    id integer NOT NULL,
    typeid integer NOT NULL,
    qntd integer NOT NULL
);


ALTER TABLE pangya.pangya_memorial_shop_luckyset OWNER TO postgres;

--
-- TOC entry 369 (class 1259 OID 24457)
-- Name: pangya_memorial_shop_normal_item; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_memorial_shop_normal_item (
    nome character varying(50) DEFAULT NULL::character varying,
    typeid integer NOT NULL,
    qntd integer NOT NULL,
    probabilidade integer NOT NULL,
    tipo integer NOT NULL
);


ALTER TABLE pangya.pangya_memorial_shop_normal_item OWNER TO postgres;

--
-- TOC entry 370 (class 1259 OID 24461)
-- Name: pangya_memorial_shop_rare_item; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_memorial_shop_rare_item (
    gacha_num integer NOT NULL,
    typeid integer NOT NULL,
    probabilidade integer NOT NULL,
    tipo integer NOT NULL,
    tipo_item integer NOT NULL,
    sexo integer NOT NULL,
    valor_1 integer NOT NULL,
    outro_item integer NOT NULL,
    character_type integer NOT NULL,
    flag smallint NOT NULL
);


ALTER TABLE pangya.pangya_memorial_shop_rare_item OWNER TO postgres;

--
-- TOC entry 371 (class 1259 OID 24464)
-- Name: pangya_memorial_shop_rate; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_memorial_shop_rate (
    nome character varying(50) DEFAULT NULL::character varying,
    probabilidade integer NOT NULL,
    tipo integer NOT NULL
);


ALTER TABLE pangya.pangya_memorial_shop_rate OWNER TO postgres;

--
-- TOC entry 372 (class 1259 OID 24468)
-- Name: pangya_msg_user; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_msg_user (
    msg_idx bigint NOT NULL,
    uid integer DEFAULT 0 NOT NULL,
    uid_from integer DEFAULT 0 NOT NULL,
    valid smallint DEFAULT 1 NOT NULL,
    msg character varying(500) DEFAULT 'hello'::character varying NOT NULL,
    reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_msg_user OWNER TO postgres;

--
-- TOC entry 373 (class 1259 OID 24479)
-- Name: pangya_msg_user_msg_idx_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_msg_user ALTER COLUMN msg_idx ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_msg_user_msg_idx_seq
    START WITH 54
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 374 (class 1259 OID 24481)
-- Name: pangya_myroom; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_myroom (
    uid integer NOT NULL,
    senha character varying(15) DEFAULT NULL::character varying,
    public_lock smallint DEFAULT 0 NOT NULL,
    state smallint DEFAULT 1 NOT NULL
);


ALTER TABLE pangya.pangya_myroom OWNER TO postgres;

--
-- TOC entry 375 (class 1259 OID 24487)
-- Name: pangya_new_box; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_new_box (
    id integer NOT NULL,
    nome character varying(100),
    typeid integer NOT NULL,
    opened_typeid integer DEFAULT 0 NOT NULL,
    numero integer DEFAULT 1 NOT NULL,
    tipo_open smallint DEFAULT 0 NOT NULL,
    tipo smallint DEFAULT 0 NOT NULL,
    message character varying(80) DEFAULT 'OUUUU VOCÊ GANHOU UM ITEM<SUPERSS>'::character varying NOT NULL,
    active smallint DEFAULT 1 NOT NULL
);


ALTER TABLE pangya.pangya_new_box OWNER TO postgres;

--
-- TOC entry 376 (class 1259 OID 24496)
-- Name: pangya_new_box_id_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_new_box ALTER COLUMN id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_new_box_id_seq
    START WITH 50
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 377 (class 1259 OID 24498)
-- Name: pangya_new_box_item; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_new_box_item (
    index bigint NOT NULL,
    box_id integer NOT NULL,
    nome character(100),
    typeid integer NOT NULL,
    numero integer DEFAULT '-1'::integer NOT NULL,
    probabilidade integer DEFAULT 100 NOT NULL,
    qntd integer DEFAULT 1 NOT NULL,
    raridade smallint DEFAULT 0 NOT NULL,
    duplicar smallint DEFAULT 0 NOT NULL,
    active smallint DEFAULT 1 NOT NULL
);


ALTER TABLE pangya.pangya_new_box_item OWNER TO postgres;

--
-- TOC entry 378 (class 1259 OID 24507)
-- Name: pangya_new_box_item_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_new_box_item ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_new_box_item_index_seq
    START WITH 1481
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 379 (class 1259 OID 24509)
-- Name: pangya_new_box_rare_win_log; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_new_box_rare_win_log (
    index bigint NOT NULL,
    uid integer NOT NULL,
    box_typeid integer NOT NULL,
    item_typeid integer NOT NULL,
    qntd integer NOT NULL,
    raridade smallint NOT NULL,
    win_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_new_box_rare_win_log OWNER TO postgres;

--
-- TOC entry 380 (class 1259 OID 24513)
-- Name: pangya_new_box_rare_win_log_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_new_box_rare_win_log ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_new_box_rare_win_log_index_seq
    START WITH 262
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 381 (class 1259 OID 24515)
-- Name: pangya_new_card_pack; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_new_card_pack (
    index integer NOT NULL,
    name character varying(100),
    typeid integer NOT NULL,
    quantidade smallint DEFAULT 1 NOT NULL,
    tipo smallint NOT NULL,
    rate_n smallint DEFAULT 100 NOT NULL,
    rate_r smallint DEFAULT 100 NOT NULL,
    rate_sr smallint DEFAULT 100 NOT NULL,
    rate_sc smallint DEFAULT 100 NOT NULL
);


ALTER TABLE pangya.pangya_new_card_pack OWNER TO postgres;

--
-- TOC entry 382 (class 1259 OID 24523)
-- Name: pangya_new_card_pack_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_new_card_pack ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_new_card_pack_index_seq
    START WITH 85
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 383 (class 1259 OID 24525)
-- Name: pangya_new_cards; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_new_cards (
    index integer NOT NULL,
    name character varying(100),
    typeid integer NOT NULL,
    probabilidade integer NOT NULL,
    tipo smallint NOT NULL,
    pack smallint NOT NULL
);


ALTER TABLE pangya.pangya_new_cards OWNER TO postgres;

--
-- TOC entry 384 (class 1259 OID 24528)
-- Name: pangya_new_cards_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_new_cards ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_new_cards_index_seq
    START WITH 2177
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 385 (class 1259 OID 24530)
-- Name: pangya_new_course_drop; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_new_course_drop (
    index bigint NOT NULL,
    rate_mana_artefact integer DEFAULT 100 NOT NULL,
    rate_grand_prix_ticket integer DEFAULT 100 NOT NULL,
    rate_ssc_ticket integer DEFAULT 100 NOT NULL
);


ALTER TABLE pangya.pangya_new_course_drop OWNER TO postgres;

--
-- TOC entry 386 (class 1259 OID 24536)
-- Name: pangya_new_course_drop_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_new_course_drop ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_new_course_drop_index_seq
    START WITH 2
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 387 (class 1259 OID 24538)
-- Name: pangya_new_course_drop_item; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_new_course_drop_item (
    index bigint NOT NULL,
    course smallint NOT NULL,
    tipo smallint DEFAULT 0 NOT NULL,
    typeid integer NOT NULL,
    quantidade integer DEFAULT 1 NOT NULL,
    probabilidade_3h integer DEFAULT 0 NOT NULL,
    probabilidade_6h integer DEFAULT 0 NOT NULL,
    probabilidade_9h integer DEFAULT 0 NOT NULL,
    probabilidade_18h integer DEFAULT 0 NOT NULL,
    active smallint DEFAULT 1 NOT NULL
);


ALTER TABLE pangya.pangya_new_course_drop_item OWNER TO postgres;

--
-- TOC entry 388 (class 1259 OID 24548)
-- Name: pangya_new_course_drop_item_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_new_course_drop_item ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_new_course_drop_item_index_seq
    START WITH 46
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 389 (class 1259 OID 24550)
-- Name: pangya_new_memorial_level; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_new_memorial_level (
    level integer NOT NULL,
    gacha_start integer NOT NULL,
    gacha_end integer NOT NULL
);


ALTER TABLE pangya.pangya_new_memorial_level OWNER TO postgres;

--
-- TOC entry 390 (class 1259 OID 24553)
-- Name: pangya_new_memorial_lucky_set; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_new_memorial_lucky_set (
    index bigint NOT NULL,
    set_id integer NOT NULL,
    nome character varying(100),
    typeid integer NOT NULL,
    qntd integer NOT NULL
);


ALTER TABLE pangya.pangya_new_memorial_lucky_set OWNER TO postgres;

--
-- TOC entry 391 (class 1259 OID 24556)
-- Name: pangya_new_memorial_lucky_set_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_new_memorial_lucky_set ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_new_memorial_lucky_set_index_seq
    START WITH 36
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 392 (class 1259 OID 24558)
-- Name: pangya_new_memorial_normal_item; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_new_memorial_normal_item (
    index bigint NOT NULL,
    nome character varying(100),
    typeid integer NOT NULL,
    qntd integer NOT NULL,
    tipo smallint DEFAULT 0 NOT NULL,
    active smallint DEFAULT 1 NOT NULL
);


ALTER TABLE pangya.pangya_new_memorial_normal_item OWNER TO postgres;

--
-- TOC entry 393 (class 1259 OID 24563)
-- Name: pangya_new_memorial_normal_item_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_new_memorial_normal_item ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_new_memorial_normal_item_index_seq
    START WITH 15
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 394 (class 1259 OID 24565)
-- Name: pangya_new_memorial_rare_win_log; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_new_memorial_rare_win_log (
    index bigint NOT NULL,
    uid integer NOT NULL,
    coin_typeid integer NOT NULL,
    item_typeid integer NOT NULL,
    item_qntd integer NOT NULL,
    item_raridade integer NOT NULL,
    item_probabilidade integer NOT NULL,
    win_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


ALTER TABLE pangya.pangya_new_memorial_rare_win_log OWNER TO postgres;

--
-- TOC entry 395 (class 1259 OID 24569)
-- Name: pangya_new_memorial_rare_win_log_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_new_memorial_rare_win_log ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_new_memorial_rare_win_log_index_seq
    START WITH 112
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 396 (class 1259 OID 24571)
-- Name: pangya_notice_list; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_notice_list (
    notice_id bigint NOT NULL,
    message character varying(1024) DEFAULT NULL::character varying,
    replaycount integer DEFAULT 1 NOT NULL,
    refreshtime integer DEFAULT 0 NOT NULL
);


ALTER TABLE pangya.pangya_notice_list OWNER TO postgres;

--
-- TOC entry 397 (class 1259 OID 24580)
-- Name: pangya_notice_list_notice_id_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_notice_list ALTER COLUMN notice_id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_notice_list_notice_id_seq
    START WITH 260
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 398 (class 1259 OID 24582)
-- Name: pangya_papel_shop_config; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_papel_shop_config (
    numero integer DEFAULT 1 NOT NULL,
    price_normal bigint DEFAULT 900 NOT NULL,
    price_big bigint DEFAULT 10000 NOT NULL,
    limitted_yn smallint DEFAULT 1 NOT NULL,
    update_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


ALTER TABLE pangya.pangya_papel_shop_config OWNER TO postgres;

--
-- TOC entry 399 (class 1259 OID 24590)
-- Name: pangya_papel_shop_coupon; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_papel_shop_coupon (
    index bigint NOT NULL,
    typeid integer NOT NULL,
    active smallint DEFAULT 1 NOT NULL
);


ALTER TABLE pangya.pangya_papel_shop_coupon OWNER TO postgres;

--
-- TOC entry 400 (class 1259 OID 24594)
-- Name: pangya_papel_shop_coupon_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_papel_shop_coupon ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_papel_shop_coupon_index_seq
    START WITH 4
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 401 (class 1259 OID 24596)
-- Name: pangya_papel_shop_info; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_papel_shop_info (
    index bigint NOT NULL,
    uid integer NOT NULL,
    current_cnt smallint DEFAULT 0 NOT NULL,
    remain_cnt smallint DEFAULT 0 NOT NULL,
    limit_cnt smallint DEFAULT 50 NOT NULL,
    last_update timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


ALTER TABLE pangya.pangya_papel_shop_info OWNER TO postgres;

--
-- TOC entry 402 (class 1259 OID 24603)
-- Name: pangya_papel_shop_info_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_papel_shop_info ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_papel_shop_info_index_seq
    START WITH 10406
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 403 (class 1259 OID 24605)
-- Name: pangya_papel_shop_item; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_papel_shop_item (
    index bigint NOT NULL,
    nome character varying(100),
    typeid integer NOT NULL,
    probabilidade integer NOT NULL,
    numero integer DEFAULT '-1'::integer NOT NULL,
    tipo smallint NOT NULL,
    active smallint DEFAULT 1 NOT NULL
);


ALTER TABLE pangya.pangya_papel_shop_item OWNER TO postgres;

--
-- TOC entry 404 (class 1259 OID 24610)
-- Name: pangya_papel_shop_item_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_papel_shop_item ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_papel_shop_item_index_seq
    START WITH 392
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 405 (class 1259 OID 24612)
-- Name: pangya_papel_shop_rare_win_log; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_papel_shop_rare_win_log (
    index bigint NOT NULL,
    uid integer NOT NULL,
    typeid integer NOT NULL,
    qntd integer NOT NULL,
    ball_color smallint NOT NULL,
    probabilidade integer NOT NULL,
    reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_papel_shop_rare_win_log OWNER TO postgres;

--
-- TOC entry 406 (class 1259 OID 24616)
-- Name: pangya_papel_shop_rare_win_log_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_papel_shop_rare_win_log ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_papel_shop_rare_win_log_index_seq
    START WITH 163
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 407 (class 1259 OID 24618)
-- Name: pangya_parts_list; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_parts_list (
    index bigint NOT NULL,
    typeid integer DEFAULT 0 NOT NULL,
    tipo integer DEFAULT 0 NOT NULL,
    equip_flag integer DEFAULT 0 NOT NULL
);


ALTER TABLE pangya.pangya_parts_list OWNER TO postgres;

--
-- TOC entry 408 (class 1259 OID 24624)
-- Name: pangya_parts_list_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_parts_list ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_parts_list_index_seq
    START WITH 7326
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 409 (class 1259 OID 24626)
-- Name: pangya_personal_shop_log; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_personal_shop_log (
    index bigint NOT NULL,
    player_sell_uid integer NOT NULL,
    player_buy_uid integer NOT NULL,
    item_typeid integer NOT NULL,
    item_id_sell integer NOT NULL,
    item_id_buy integer NOT NULL,
    item_qntd integer NOT NULL,
    item_pang bigint NOT NULL,
    total_pang bigint NOT NULL,
    reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_personal_shop_log OWNER TO postgres;

--
-- TOC entry 410 (class 1259 OID 24630)
-- Name: pangya_personal_shop_log_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_personal_shop_log ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_personal_shop_log_index_seq
    START WITH 79
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 411 (class 1259 OID 24632)
-- Name: pangya_player_ip; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_player_ip (
    index bigint NOT NULL,
    uid integer DEFAULT 0 NOT NULL,
    ip character varying(18) DEFAULT '000.000.000.000'::character varying NOT NULL,
    block_beta smallint DEFAULT 1 NOT NULL,
    flag_day smallint DEFAULT 1 NOT NULL,
    change_count integer DEFAULT 0 NOT NULL,
    change_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


ALTER TABLE pangya.pangya_player_ip OWNER TO postgres;

--
-- TOC entry 412 (class 1259 OID 24641)
-- Name: pangya_player_ip_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_player_ip ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_player_ip_index_seq
    START WITH 10158
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 413 (class 1259 OID 24643)
-- Name: pangya_player_location; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_player_location (
    uid integer NOT NULL,
    channel smallint DEFAULT '-1'::integer NOT NULL,
    lobby smallint DEFAULT '-1'::integer NOT NULL,
    room smallint DEFAULT '-1'::integer NOT NULL,
    place smallint DEFAULT 0 NOT NULL
);


ALTER TABLE pangya.pangya_player_location OWNER TO postgres;

--
-- TOC entry 414 (class 1259 OID 24650)
-- Name: pangya_quest; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_quest (
    id integer NOT NULL,
    achievement_id integer NOT NULL,
    uid integer NOT NULL,
    name character varying(100),
    typeid integer NOT NULL,
    counter_item_id integer DEFAULT 0 NOT NULL,
    date timestamp without time zone
);


ALTER TABLE pangya.pangya_quest OWNER TO postgres;

--
-- TOC entry 415 (class 1259 OID 24654)
-- Name: pangya_quest_clear; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_quest_clear (
    index bigint NOT NULL,
    uid integer DEFAULT 0 NOT NULL,
    quest_id integer DEFAULT 0 NOT NULL,
    option smallint DEFAULT 1 NOT NULL
);


ALTER TABLE pangya.pangya_quest_clear OWNER TO postgres;

--
-- TOC entry 416 (class 1259 OID 24660)
-- Name: pangya_quest_clear_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_quest_clear ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_quest_clear_index_seq
    START WITH 55029
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 417 (class 1259 OID 24662)
-- Name: pangya_quest_id_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_quest ALTER COLUMN id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_quest_id_seq
    START WITH 535336
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 418 (class 1259 OID 24664)
-- Name: pangya_rank_antes; Type: TABLE; Schema: pangya; Owner: pangya
--

CREATE TABLE pangya.pangya_rank_antes (
    index bigint NOT NULL,
    "position" integer DEFAULT 0 NOT NULL,
    uid integer DEFAULT 0 NOT NULL,
    tipo_rank smallint DEFAULT 0 NOT NULL,
    tipo_rank_seq smallint DEFAULT 0 NOT NULL,
    valor integer DEFAULT 0 NOT NULL,
    reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


ALTER TABLE pangya.pangya_rank_antes OWNER TO pangya;

--
-- TOC entry 419 (class 1259 OID 24673)
-- Name: pangya_rank_antes_index_seq; Type: SEQUENCE; Schema: pangya; Owner: pangya
--

ALTER TABLE pangya.pangya_rank_antes ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_rank_antes_index_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 420 (class 1259 OID 24675)
-- Name: pangya_rank_atual; Type: TABLE; Schema: pangya; Owner: pangya
--

CREATE TABLE pangya.pangya_rank_atual (
    index bigint NOT NULL,
    "position" integer DEFAULT 0 NOT NULL,
    uid integer DEFAULT 0 NOT NULL,
    tipo_rank smallint DEFAULT 0 NOT NULL,
    tipo_rank_seq smallint DEFAULT 0 NOT NULL,
    valor integer DEFAULT 0 NOT NULL,
    reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


ALTER TABLE pangya.pangya_rank_atual OWNER TO pangya;

--
-- TOC entry 421 (class 1259 OID 24684)
-- Name: pangya_rank_atual_character; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_rank_atual_character (
    uid integer NOT NULL,
    item_id integer DEFAULT 0 NOT NULL,
    typeid integer DEFAULT 0 NOT NULL,
    itemid_parts_1 integer DEFAULT 0 NOT NULL,
    itemid_parts_2 integer DEFAULT 0 NOT NULL,
    itemid_parts_3 integer DEFAULT 0 NOT NULL,
    itemid_parts_4 integer DEFAULT 0 NOT NULL,
    itemid_parts_5 integer DEFAULT 0 NOT NULL,
    itemid_parts_6 integer DEFAULT 0 NOT NULL,
    itemid_parts_7 integer DEFAULT 0 NOT NULL,
    itemid_parts_8 integer DEFAULT 0 NOT NULL,
    itemid_parts_9 integer DEFAULT 0 NOT NULL,
    itemid_parts_10 integer DEFAULT 0 NOT NULL,
    itemid_parts_11 integer DEFAULT 0 NOT NULL,
    itemid_parts_12 integer DEFAULT 0 NOT NULL,
    itemid_parts_13 integer DEFAULT 0 NOT NULL,
    itemid_parts_14 integer DEFAULT 0 NOT NULL,
    itemid_parts_15 integer DEFAULT 0 NOT NULL,
    itemid_parts_16 integer DEFAULT 0 NOT NULL,
    itemid_parts_17 integer DEFAULT 0 NOT NULL,
    itemid_parts_18 integer DEFAULT 0 NOT NULL,
    itemid_parts_19 integer DEFAULT 0 NOT NULL,
    itemid_parts_20 integer DEFAULT 0 NOT NULL,
    itemid_parts_21 integer DEFAULT 0 NOT NULL,
    itemid_parts_22 integer DEFAULT 0 NOT NULL,
    itemid_parts_23 integer DEFAULT 0 NOT NULL,
    itemid_parts_24 integer DEFAULT 0 NOT NULL,
    parts_1 integer DEFAULT 0 NOT NULL,
    parts_2 integer DEFAULT 0 NOT NULL,
    parts_3 integer DEFAULT 0 NOT NULL,
    parts_4 integer DEFAULT 0 NOT NULL,
    parts_5 integer DEFAULT 0 NOT NULL,
    parts_6 integer DEFAULT 0 NOT NULL,
    parts_7 integer DEFAULT 0 NOT NULL,
    parts_8 integer DEFAULT 0 NOT NULL,
    parts_9 integer DEFAULT 0 NOT NULL,
    parts_10 integer DEFAULT 0 NOT NULL,
    parts_11 integer DEFAULT 0 NOT NULL,
    parts_12 integer DEFAULT 0 NOT NULL,
    parts_13 integer DEFAULT 0 NOT NULL,
    parts_14 integer DEFAULT 0 NOT NULL,
    parts_15 integer DEFAULT 0 NOT NULL,
    parts_16 integer DEFAULT 0 NOT NULL,
    parts_17 integer DEFAULT 0 NOT NULL,
    parts_18 integer DEFAULT 0 NOT NULL,
    parts_19 integer DEFAULT 0 NOT NULL,
    parts_20 integer DEFAULT 0 NOT NULL,
    parts_21 integer DEFAULT 0 NOT NULL,
    parts_22 integer DEFAULT 0 NOT NULL,
    parts_23 integer DEFAULT 0 NOT NULL,
    parts_24 integer DEFAULT 0 NOT NULL,
    default_hair smallint DEFAULT 0 NOT NULL,
    default_shirts smallint DEFAULT 0 NOT NULL,
    gift_flag smallint DEFAULT 0 NOT NULL,
    pcl0 smallint DEFAULT 0 NOT NULL,
    pcl1 smallint DEFAULT 0 NOT NULL,
    pcl2 smallint DEFAULT 0 NOT NULL,
    pcl3 smallint DEFAULT 0 NOT NULL,
    pcl4 smallint DEFAULT 0 NOT NULL,
    purchase smallint DEFAULT 0 NOT NULL,
    auxparts_1 integer DEFAULT 0 NOT NULL,
    auxparts_2 integer DEFAULT 0 NOT NULL,
    auxparts_3 integer DEFAULT 0 NOT NULL,
    auxparts_4 integer DEFAULT 0 NOT NULL,
    auxparts_5 integer DEFAULT 0 NOT NULL,
    cutin_1 integer DEFAULT 0 NOT NULL,
    cutin_2 integer DEFAULT 0 NOT NULL,
    cutin_3 integer DEFAULT 0 NOT NULL,
    cutin_4 integer DEFAULT 0 NOT NULL,
    mastery integer DEFAULT 0 NOT NULL,
    card_character_1 integer DEFAULT 0 NOT NULL,
    card_character_2 integer DEFAULT 0 NOT NULL,
    card_character_3 integer DEFAULT 0 NOT NULL,
    card_character_4 integer DEFAULT 0 NOT NULL,
    card_caddie_1 integer DEFAULT 0 NOT NULL,
    card_caddie_2 integer DEFAULT 0 NOT NULL,
    card_caddie_3 integer DEFAULT 0 NOT NULL,
    card_caddie_4 integer DEFAULT 0 NOT NULL,
    card_npc_1 integer DEFAULT 0 NOT NULL,
    card_npc_2 integer DEFAULT 0 NOT NULL,
    card_npc_3 integer DEFAULT 0 NOT NULL,
    card_npc_4 integer DEFAULT 0 NOT NULL
);


ALTER TABLE pangya.pangya_rank_atual_character OWNER TO postgres;

--
-- TOC entry 422 (class 1259 OID 24768)
-- Name: pangya_rank_atual_index_seq; Type: SEQUENCE; Schema: pangya; Owner: pangya
--

ALTER TABLE pangya.pangya_rank_atual ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_rank_atual_index_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 423 (class 1259 OID 24770)
-- Name: pangya_rank_config; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_rank_config (
    index bigint NOT NULL,
    refresh_time_h integer DEFAULT 0 NOT NULL,
    reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP
);


ALTER TABLE pangya.pangya_rank_config OWNER TO postgres;

--
-- TOC entry 424 (class 1259 OID 24775)
-- Name: pangya_rank_config_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_rank_config ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_rank_config_index_seq
    START WITH 2
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 425 (class 1259 OID 24777)
-- Name: pangya_record; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_record (
    uid integer NOT NULL,
    tipo smallint DEFAULT 0 NOT NULL,
    course smallint DEFAULT 0 NOT NULL,
    best_score smallint DEFAULT 127 NOT NULL,
    best_pang bigint DEFAULT 0 NOT NULL,
    character_typeid integer DEFAULT 0 NOT NULL,
    event_score smallint DEFAULT 0 NOT NULL,
    tacada integer DEFAULT 0 NOT NULL,
    putt integer DEFAULT 0 NOT NULL,
    hole integer DEFAULT 0 NOT NULL,
    fairway integer DEFAULT 0 NOT NULL,
    puttin integer DEFAULT 0 NOT NULL,
    total_score integer DEFAULT 0 NOT NULL,
    holein integer DEFAULT 0 NOT NULL,
    assist smallint DEFAULT 0 NOT NULL
);


ALTER TABLE pangya.pangya_record OWNER TO postgres;

--
-- TOC entry 426 (class 1259 OID 24794)
-- Name: pangya_rescue_pwd_log; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_rescue_pwd_log (
    index bigint NOT NULL,
    uid integer NOT NULL,
    tipo smallint DEFAULT 1 NOT NULL,
    key_uniq uuid DEFAULT gen_random_uuid() NOT NULL,
    state smallint DEFAULT 0 NOT NULL,
    send_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_rescue_pwd_log OWNER TO postgres;

--
-- TOC entry 427 (class 1259 OID 24801)
-- Name: pangya_rescue_pwd_log_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_rescue_pwd_log ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_rescue_pwd_log_index_seq
    START WITH 55
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 428 (class 1259 OID 24803)
-- Name: pangya_reward_ssc; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_reward_ssc (
    valor integer DEFAULT 0 NOT NULL,
    probabilidade integer DEFAULT 0 NOT NULL,
    tipo integer DEFAULT 0 NOT NULL
);


ALTER TABLE pangya.pangya_reward_ssc OWNER TO postgres;

--
-- TOC entry 429 (class 1259 OID 24809)
-- Name: pangya_scratchy_prob_sec; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_scratchy_prob_sec (
    uid integer NOT NULL,
    scratchy_sec integer DEFAULT 0 NOT NULL
);


ALTER TABLE pangya.pangya_scratchy_prob_sec OWNER TO postgres;

--
-- TOC entry 430 (class 1259 OID 24813)
-- Name: pangya_server_list; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_server_list (
    name character varying(50) DEFAULT 'JaCk2 Server'::character varying NOT NULL,
    uid integer NOT NULL,
    ip character varying(20) NOT NULL,
    port integer NOT NULL,
    maxuser integer NOT NULL,
    curruser integer NOT NULL,
    type smallint NOT NULL,
    updatetime timestamp without time zone NOT NULL,
    state smallint NOT NULL,
    pcbanguser smallint NOT NULL,
    pangrate integer NOT NULL,
    serverversion character varying(40) DEFAULT ''::character varying NOT NULL,
    clientversion character varying(20) NOT NULL,
    property integer NOT NULL,
    angelicwingsnum integer NOT NULL,
    eventflag smallint NOT NULL,
    exprate integer NOT NULL,
    rareitemrate integer NOT NULL,
    cookieitemrate integer NOT NULL,
    servicecontrol integer NOT NULL,
    imgno smallint NOT NULL,
    apprate smallint NOT NULL,
    scratchrate smallint NOT NULL,
    eventmap integer NOT NULL,
    eventdroprate integer NOT NULL,
    hanbituser integer NOT NULL,
    paranuser integer NOT NULL,
    authstate smallint NOT NULL,
    masteryrate smallint NOT NULL,
    treasurerate smallint NOT NULL,
    chuvarate smallint NOT NULL
);


ALTER TABLE pangya.pangya_server_list OWNER TO postgres;

--
-- TOC entry 431 (class 1259 OID 24818)
-- Name: pangya_shutdown_list; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_shutdown_list (
    id bigint NOT NULL,
    date_shutdown timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    replaycount integer DEFAULT 1 NOT NULL,
    refreshtime integer DEFAULT 0 NOT NULL
);


ALTER TABLE pangya.pangya_shutdown_list OWNER TO postgres;

--
-- TOC entry 432 (class 1259 OID 24824)
-- Name: pangya_shutdown_list_id_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_shutdown_list ALTER COLUMN id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_shutdown_list_id_seq
    START WITH 192
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 433 (class 1259 OID 24826)
-- Name: pangya_ticker_list; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_ticker_list (
    ticker_id bigint NOT NULL,
    message character varying(500) NOT NULL,
    nick character varying(22) NOT NULL,
    replaycount integer DEFAULT 1 NOT NULL,
    refreshtime integer DEFAULT 0 NOT NULL
);


ALTER TABLE pangya.pangya_ticker_list OWNER TO postgres;

--
-- TOC entry 434 (class 1259 OID 24834)
-- Name: pangya_ticker_list_ticker_id_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_ticker_list ALTER COLUMN ticker_id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_ticker_list_ticker_id_seq
    START WITH 425
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 435 (class 1259 OID 24836)
-- Name: pangya_ticket_report; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_ticket_report (
    idx bigint NOT NULL,
    trofel_typeid integer DEFAULT 738197504 NOT NULL,
    flag smallint DEFAULT 0 NOT NULL,
    reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    tipo integer DEFAULT 4 NOT NULL
);


ALTER TABLE pangya.pangya_ticket_report OWNER TO postgres;

--
-- TOC entry 436 (class 1259 OID 24843)
-- Name: pangya_ticket_report_dados; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_ticket_report_dados (
    report_id bigint NOT NULL,
    player_uid integer NOT NULL,
    player_score smallint DEFAULT 0 NOT NULL,
    player_medalha smallint DEFAULT 0 NOT NULL,
    player_trofel smallint DEFAULT 0 NOT NULL,
    player_pang bigint DEFAULT 0 NOT NULL,
    player_bonus_pang bigint DEFAULT 0 NOT NULL,
    player_exp integer DEFAULT 0 NOT NULL,
    player_mascot_typeid integer DEFAULT 0 NOT NULL,
    player_state smallint DEFAULT 0 NOT NULL,
    flag_item_pang smallint DEFAULT 0 NOT NULL,
    flag_premium_user smallint DEFAULT 0 NOT NULL,
    finish_date timestamp without time zone
);


ALTER TABLE pangya.pangya_ticket_report_dados OWNER TO postgres;

--
-- TOC entry 437 (class 1259 OID 24856)
-- Name: pangya_ticket_report_idx_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_ticket_report ALTER COLUMN idx ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_ticket_report_idx_seq
    START WITH 491
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 438 (class 1259 OID 24858)
-- Name: pangya_tiki_points; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_tiki_points (
    uid integer NOT NULL,
    tiki_points bigint DEFAULT 0 NOT NULL,
    reg_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL,
    mod_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL
);


ALTER TABLE pangya.pangya_tiki_points OWNER TO postgres;

--
-- TOC entry 439 (class 1259 OID 24864)
-- Name: pangya_transforme_clubset_temp; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_transforme_clubset_temp (
    trans_index bigint NOT NULL,
    uid integer DEFAULT 0 NOT NULL,
    taqueira_id integer DEFAULT 0 NOT NULL,
    state integer DEFAULT 0 NOT NULL,
    mastery integer DEFAULT 0 NOT NULL,
    state2 integer DEFAULT 0 NOT NULL,
    flag smallint DEFAULT 0 NOT NULL,
    card_typeid integer DEFAULT 0 NOT NULL,
    card_qntd integer DEFAULT 0 NOT NULL,
    taqueira_trans_typeid integer DEFAULT 0 NOT NULL
);


ALTER TABLE pangya.pangya_transforme_clubset_temp OWNER TO postgres;

--
-- TOC entry 440 (class 1259 OID 24876)
-- Name: pangya_transforme_clubset_temp_trans_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_transforme_clubset_temp ALTER COLUMN trans_index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_transforme_clubset_temp_trans_index_seq
    START WITH 21
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 441 (class 1259 OID 24878)
-- Name: pangya_treasure_item; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_treasure_item (
    name character varying(50) DEFAULT NULL::character varying,
    typeid integer NOT NULL,
    quantidade integer NOT NULL,
    probabilidade integer NOT NULL,
    tipo integer NOT NULL,
    flag smallint NOT NULL
);


ALTER TABLE pangya.pangya_treasure_item OWNER TO postgres;

--
-- TOC entry 442 (class 1259 OID 24882)
-- Name: pangya_trofel_especial; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_trofel_especial (
    item_id bigint NOT NULL,
    uid integer DEFAULT 0 NOT NULL,
    typeid integer DEFAULT 0 NOT NULL,
    qntd integer DEFAULT 0 NOT NULL
);


ALTER TABLE pangya.pangya_trofel_especial OWNER TO postgres;

--
-- TOC entry 443 (class 1259 OID 24888)
-- Name: pangya_trofel_especial_item_id_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_trofel_especial ALTER COLUMN item_id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_trofel_especial_item_id_seq
    START WITH 7
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 444 (class 1259 OID 24890)
-- Name: pangya_trofel_grandprix; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_trofel_grandprix (
    item_id bigint NOT NULL,
    uid integer DEFAULT 0 NOT NULL,
    typeid integer DEFAULT 0 NOT NULL,
    qntd integer DEFAULT 0 NOT NULL
);


ALTER TABLE pangya.pangya_trofel_grandprix OWNER TO postgres;

--
-- TOC entry 445 (class 1259 OID 24896)
-- Name: pangya_trofel_grandprix_item_id_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_trofel_grandprix ALTER COLUMN item_id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_trofel_grandprix_item_id_seq
    START WITH 24
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 446 (class 1259 OID 24898)
-- Name: pangya_user_equip; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_user_equip (
    uid integer NOT NULL,
    caddie_id integer DEFAULT 0 NOT NULL,
    character_id integer DEFAULT 0 NOT NULL,
    club_id integer DEFAULT 0 NOT NULL,
    ball_type integer DEFAULT 0 NOT NULL,
    item_slot_1 integer DEFAULT 0 NOT NULL,
    item_slot_2 integer DEFAULT 0 NOT NULL,
    item_slot_3 integer DEFAULT 0 NOT NULL,
    item_slot_4 integer DEFAULT 0 NOT NULL,
    item_slot_5 integer DEFAULT 0 NOT NULL,
    item_slot_6 integer DEFAULT 0 NOT NULL,
    item_slot_7 integer DEFAULT 0 NOT NULL,
    item_slot_8 integer DEFAULT 0 NOT NULL,
    item_slot_9 integer DEFAULT 0 NOT NULL,
    item_slot_10 integer DEFAULT 0 NOT NULL,
    skin_1 integer DEFAULT 0 NOT NULL,
    skin_2 integer DEFAULT 0 NOT NULL,
    skin_3 integer DEFAULT 0 NOT NULL,
    skin_4 integer DEFAULT 0 NOT NULL,
    skin_5 integer DEFAULT 0 NOT NULL,
    skin_6 integer DEFAULT 0 NOT NULL,
    mascot_id integer DEFAULT 0 NOT NULL,
    poster_1 integer DEFAULT 0 NOT NULL,
    poster_2 integer DEFAULT 0 NOT NULL
);


ALTER TABLE pangya.pangya_user_equip OWNER TO postgres;

--
-- TOC entry 447 (class 1259 OID 24924)
-- Name: pangya_user_macro; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_user_macro (
    uid integer NOT NULL,
    macro1 character varying(500) DEFAULT 'Pangya!'::character varying,
    macro2 character varying(500) DEFAULT 'Pangya!'::character varying,
    macro3 character varying(500) DEFAULT 'Pangya!'::character varying,
    macro4 character varying(500) DEFAULT 'Pangya!'::character varying,
    macro5 character varying(500) DEFAULT 'Pangya!'::character varying,
    macro6 character varying(500) DEFAULT 'Pangya!'::character varying,
    macro7 character varying(500) DEFAULT 'Pangya!'::character varying,
    macro8 character varying(500) DEFAULT 'Pangya!'::character varying,
    macro9 character varying(500) DEFAULT 'Pangya!'::character varying,
    macro10 character varying(500) DEFAULT 'Pangya!'::character varying
);


ALTER TABLE pangya.pangya_user_macro OWNER TO postgres;

--
-- TOC entry 448 (class 1259 OID 24940)
-- Name: pangya_weblink_key; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.pangya_weblink_key (
    index bigint NOT NULL,
    uid integer DEFAULT 0 NOT NULL,
    key character varying(8) DEFAULT '123456'::character varying NOT NULL,
    valid smallint DEFAULT 1 NOT NULL
);


ALTER TABLE pangya.pangya_weblink_key OWNER TO postgres;

--
-- TOC entry 449 (class 1259 OID 24946)
-- Name: pangya_weblink_key_index_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.pangya_weblink_key ALTER COLUMN index ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.pangya_weblink_key_index_seq
    START WITH 75
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 450 (class 1259 OID 24948)
-- Name: td_char_equip_s4; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.td_char_equip_s4 (
    seq bigint NOT NULL,
    uid integer NOT NULL,
    char_itemid integer DEFAULT 0 NOT NULL,
    itemid integer DEFAULT 0 NOT NULL,
    in_date timestamp without time zone,
    equip_num integer DEFAULT 0 NOT NULL,
    equip_type integer DEFAULT 0 NOT NULL,
    use_yn character varying(1) DEFAULT 'Y'::character varying
);


ALTER TABLE pangya.td_char_equip_s4 OWNER TO postgres;

--
-- TOC entry 451 (class 1259 OID 24956)
-- Name: td_char_equip_s4_seq_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.td_char_equip_s4 ALTER COLUMN seq ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.td_char_equip_s4_seq_seq
    START WITH 114133
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 452 (class 1259 OID 24958)
-- Name: td_room_data; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.td_room_data (
    myroom_id bigint NOT NULL,
    uid integer DEFAULT 0 NOT NULL,
    room_no integer DEFAULT 0 NOT NULL,
    typeid integer DEFAULT 0 NOT NULL,
    pos_x real DEFAULT 0 NOT NULL,
    pos_y real DEFAULT 0 NOT NULL,
    pos_z real DEFAULT 0 NOT NULL,
    pos_r real DEFAULT 0 NOT NULL,
    mod_seq integer DEFAULT 0 NOT NULL,
    display_yn character varying(1) DEFAULT ''::character varying NOT NULL,
    use_yn character varying(1) DEFAULT 'Y'::character varying NOT NULL,
    mod_dt timestamp without time zone DEFAULT CURRENT_TIMESTAMP,
    valid smallint DEFAULT 1
);


ALTER TABLE pangya.td_room_data OWNER TO postgres;

--
-- TOC entry 453 (class 1259 OID 24973)
-- Name: td_room_data_myroom_id_seq; Type: SEQUENCE; Schema: pangya; Owner: postgres
--

ALTER TABLE pangya.td_room_data ALTER COLUMN myroom_id ADD GENERATED ALWAYS AS IDENTITY (
    SEQUENCE NAME pangya.td_room_data_myroom_id_seq
    START WITH 10883
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1
);


--
-- TOC entry 454 (class 1259 OID 24975)
-- Name: trofel_stat; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.trofel_stat (
    uid bigint DEFAULT 0 NOT NULL,
    ama_6_g smallint DEFAULT 0 NOT NULL,
    ama_6_s smallint DEFAULT 0 NOT NULL,
    ama_6_b smallint DEFAULT 0 NOT NULL,
    ama_5_g smallint DEFAULT 0 NOT NULL,
    ama_5_s smallint DEFAULT 0 NOT NULL,
    ama_5_b smallint DEFAULT 0 NOT NULL,
    ama_4_g smallint DEFAULT 0 NOT NULL,
    ama_4_s smallint DEFAULT 0 NOT NULL,
    ama_4_b smallint DEFAULT 0 NOT NULL,
    ama_3_g smallint DEFAULT 0 NOT NULL,
    ama_3_s smallint DEFAULT 0 NOT NULL,
    ama_3_b smallint DEFAULT 0 NOT NULL,
    ama_2_g smallint DEFAULT 0 NOT NULL,
    ama_2_s smallint DEFAULT 0 NOT NULL,
    ama_2_b smallint DEFAULT 0 NOT NULL,
    ama_1_g smallint DEFAULT 0 NOT NULL,
    ama_1_s smallint DEFAULT 0 NOT NULL,
    ama_1_b smallint DEFAULT 0 NOT NULL,
    pro_1_g smallint DEFAULT 0 NOT NULL,
    pro_1_s smallint DEFAULT 0 NOT NULL,
    pro_1_b smallint DEFAULT 0 NOT NULL,
    pro_2_g smallint DEFAULT 0 NOT NULL,
    pro_2_s smallint DEFAULT 0 NOT NULL,
    pro_2_b smallint DEFAULT 0 NOT NULL,
    pro_3_g smallint DEFAULT 0 NOT NULL,
    pro_3_s smallint DEFAULT 0 NOT NULL,
    pro_3_b smallint DEFAULT 0 NOT NULL,
    pro_4_g smallint DEFAULT 0 NOT NULL,
    pro_4_s smallint DEFAULT 0 NOT NULL,
    pro_4_b smallint DEFAULT 0 NOT NULL,
    pro_5_g smallint DEFAULT 0 NOT NULL,
    pro_5_s smallint DEFAULT 0 NOT NULL,
    pro_5_b smallint DEFAULT 0 NOT NULL,
    pro_6_g smallint DEFAULT 0 NOT NULL,
    pro_6_s smallint DEFAULT 0 NOT NULL,
    pro_6_b smallint DEFAULT 0 NOT NULL,
    pro_7_g smallint DEFAULT 0 NOT NULL,
    pro_7_s smallint DEFAULT 0 NOT NULL,
    pro_7_b smallint DEFAULT 0 NOT NULL
);


ALTER TABLE pangya.trofel_stat OWNER TO postgres;

--
-- TOC entry 455 (class 1259 OID 25018)
-- Name: tu_ucc; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.tu_ucc (
    uid bigint NOT NULL,
    typeid bigint NOT NULL,
    uccidx character varying(8) DEFAULT ''::character varying,
    seq integer DEFAULT 1 NOT NULL,
    ucc_name character varying(50) DEFAULT NULL::character varying,
    use_yn character varying(1) DEFAULT ''::character varying NOT NULL,
    in_date timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL,
    copier integer,
    item_id numeric(20,0) NOT NULL,
    copier_nick character varying(32) DEFAULT NULL::character varying,
    draw_dt timestamp without time zone,
    status smallint NOT NULL,
    flag smallint NOT NULL,
    skey character varying(20) DEFAULT NULL::character varying,
    trade smallint NOT NULL
);


ALTER TABLE pangya.tu_ucc OWNER TO postgres;

--
-- TOC entry 456 (class 1259 OID 25028)
-- Name: tutorial; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.tutorial (
    uid bigint DEFAULT 0 NOT NULL,
    rookie integer DEFAULT 0 NOT NULL,
    beginner integer DEFAULT 0 NOT NULL,
    advancer integer DEFAULT 0 NOT NULL
);


ALTER TABLE pangya.tutorial OWNER TO postgres;

--
-- TOC entry 457 (class 1259 OID 25035)
-- Name: user_info; Type: TABLE; Schema: pangya; Owner: postgres
--

CREATE TABLE pangya.user_info (
    uid bigint DEFAULT 0 NOT NULL,
    tacadas bigint DEFAULT 0 NOT NULL,
    putt bigint DEFAULT 0 NOT NULL,
    tempo bigint DEFAULT 0 NOT NULL,
    "Tempo tacadas" bigint DEFAULT 0 NOT NULL,
    max_distancia real DEFAULT 0 NOT NULL,
    acerto_pangya bigint DEFAULT 0 NOT NULL,
    bunker integer DEFAULT 0 NOT NULL,
    "O.B" bigint DEFAULT 0 NOT NULL,
    total_distancia bigint DEFAULT 0 NOT NULL,
    holes bigint DEFAULT 0 NOT NULL,
    holein integer DEFAULT 0 NOT NULL,
    hio bigint DEFAULT 0 NOT NULL,
    timeout smallint DEFAULT 0 NOT NULL,
    fairway bigint DEFAULT 0 NOT NULL,
    albatross bigint DEFAULT 0 NOT NULL,
    maconduta integer DEFAULT 0 NOT NULL,
    acerto_putt bigint DEFAULT 0 NOT NULL,
    "Long-putt" real DEFAULT 0 NOT NULL,
    "Chip-in" real DEFAULT 0 NOT NULL,
    xp bigint DEFAULT 0 NOT NULL,
    level smallint DEFAULT 0 NOT NULL,
    pang numeric(20,0) DEFAULT 0 NOT NULL,
    media_score integer DEFAULT 0 NOT NULL,
    bestscore0 smallint DEFAULT 0 NOT NULL,
    bestscore1 smallint DEFAULT 0 NOT NULL,
    bestscore2 smallint DEFAULT 0 NOT NULL,
    bestscore3 smallint DEFAULT 0 NOT NULL,
    bestscore4 smallint DEFAULT 0 NOT NULL,
    maxpang0 bigint DEFAULT 0 NOT NULL,
    maxpang1 bigint DEFAULT 0 NOT NULL,
    maxpang2 bigint DEFAULT 0 NOT NULL,
    maxpang3 bigint DEFAULT 0 NOT NULL,
    maxpang4 bigint DEFAULT 0 NOT NULL,
    sumpang bigint DEFAULT 0 NOT NULL,
    eventflag smallint DEFAULT 0 NOT NULL,
    jogado bigint DEFAULT 0 NOT NULL,
    quitado bigint DEFAULT 0 NOT NULL,
    skinpang bigint DEFAULT 0 NOT NULL,
    skinwin integer DEFAULT 0 NOT NULL,
    skinlose integer DEFAULT 0 NOT NULL,
    skinrunhole integer DEFAULT '-1'::integer NOT NULL,
    skinstrikepoint integer DEFAULT 0 NOT NULL,
    skinallincount integer DEFAULT 0 NOT NULL,
    todos_combos bigint DEFAULT 0 NOT NULL,
    combos bigint DEFAULT 0 NOT NULL,
    teamwin integer DEFAULT 0 NOT NULL,
    teamgames integer DEFAULT 0 NOT NULL,
    teamhole bigint DEFAULT 0 NOT NULL,
    ladderpoint integer DEFAULT 1000 NOT NULL,
    ladderwin integer DEFAULT 0 NOT NULL,
    ladderlose integer DEFAULT 0 NOT NULL,
    ladderdraw integer DEFAULT 0 NOT NULL,
    ladderhole integer DEFAULT 0 NOT NULL,
    eventvalue smallint DEFAULT 0 NOT NULL,
    naosei integer DEFAULT 0 NOT NULL,
    maxjogonaosei integer DEFAULT 0 NOT NULL,
    jogosnaosei integer DEFAULT 0 NOT NULL,
    gamecountseason integer DEFAULT 0 NOT NULL,
    cookie numeric(20,0) DEFAULT 0 NOT NULL,
    total_pang_win_game bigint DEFAULT 0 NOT NULL,
    lucky_medal integer DEFAULT 0 NOT NULL,
    fast_medal integer DEFAULT 0 NOT NULL,
    best_drive_medal integer DEFAULT 0 NOT NULL,
    best_chipin_medal integer DEFAULT 0 NOT NULL,
    best_puttin_medal integer DEFAULT 0 NOT NULL,
    best_recovery_medal integer DEFAULT 0 NOT NULL,
    "16bit_naosei" smallint DEFAULT 0 NOT NULL
);


ALTER TABLE pangya.user_info OWNER TO postgres;

--
-- TOC entry 4895 (class 2606 OID 25107)
-- Name: account ix_account; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.account
    ADD CONSTRAINT ix_account UNIQUE (id);


--
-- TOC entry 4914 (class 2606 OID 25109)
-- Name: pangya_1st_anniversary_player_win_cp ix_pangya_1st_aniversary_player_win_cp; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_1st_anniversary_player_win_cp
    ADD CONSTRAINT ix_pangya_1st_aniversary_player_win_cp UNIQUE (uid);


--
-- TOC entry 4930 (class 2606 OID 25111)
-- Name: pangya_auth_key ix_pangya_auth_key; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_auth_key
    ADD CONSTRAINT ix_pangya_auth_key UNIQUE (index);


--
-- TOC entry 4942 (class 2606 OID 25113)
-- Name: pangya_card_equip ix_pangya_card_equip; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_card_equip
    ADD CONSTRAINT ix_pangya_card_equip UNIQUE (index);


--
-- TOC entry 4944 (class 2606 OID 25115)
-- Name: pangya_change_nickname_log ix_pangya_change_nickname_log; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_change_nickname_log
    ADD CONSTRAINT ix_pangya_change_nickname_log UNIQUE (index);


--
-- TOC entry 4947 (class 2606 OID 25117)
-- Name: pangya_change_pwd_log ix_pangya_change_pwd_log; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_change_pwd_log
    ADD CONSTRAINT ix_pangya_change_pwd_log UNIQUE (uid);


--
-- TOC entry 4956 (class 2606 OID 25119)
-- Name: pangya_comet_refill ix_pangya_comet_refill; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_comet_refill
    ADD CONSTRAINT ix_pangya_comet_refill UNIQUE (index);


--
-- TOC entry 4960 (class 2606 OID 25121)
-- Name: pangya_config ix_pangya_config; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_config
    ADD CONSTRAINT ix_pangya_config UNIQUE (uid);


--
-- TOC entry 4962 (class 2606 OID 25123)
-- Name: pangya_cookie_point_item_log ix_pangya_cookie_point_item_log; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_cookie_point_item_log
    ADD CONSTRAINT ix_pangya_cookie_point_item_log UNIQUE (index);


--
-- TOC entry 4966 (class 2606 OID 25125)
-- Name: pangya_cookie_point_log ix_pangya_cookie_point_log; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_cookie_point_log
    ADD CONSTRAINT ix_pangya_cookie_point_log UNIQUE (id);


--
-- TOC entry 4990 (class 2606 OID 25127)
-- Name: pangya_donation_epin ix_pangya_donation_epin; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_donation_epin
    ADD CONSTRAINT ix_pangya_donation_epin UNIQUE (epin);


--
-- TOC entry 4992 (class 2606 OID 25129)
-- Name: pangya_donation_epin ix_pangya_donation_epin_1; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_donation_epin
    ADD CONSTRAINT ix_pangya_donation_epin_1 UNIQUE (donation_id);


--
-- TOC entry 4996 (class 2606 OID 25131)
-- Name: pangya_donation_item_log ix_pangya_donation_item_log; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_donation_item_log
    ADD CONSTRAINT ix_pangya_donation_item_log UNIQUE (index);


--
-- TOC entry 4999 (class 2606 OID 25133)
-- Name: pangya_donation_log ix_pangya_donation_log; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_donation_log
    ADD CONSTRAINT ix_pangya_donation_log UNIQUE (index);


--
-- TOC entry 5002 (class 2606 OID 25135)
-- Name: pangya_donation_new ix_pangya_donation_new; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_donation_new
    ADD CONSTRAINT ix_pangya_donation_new UNIQUE (code);


--
-- TOC entry 5014 (class 2606 OID 25137)
-- Name: pangya_gm_gift_web_log ix_pangya_gm_gift_web_log; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_gm_gift_web_log
    ADD CONSTRAINT ix_pangya_gm_gift_web_log UNIQUE (index);


--
-- TOC entry 5031 (class 2606 OID 25139)
-- Name: pangya_guild ix_pangya_guild; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_guild
    ADD CONSTRAINT ix_pangya_guild UNIQUE (guild_uid);


--
-- TOC entry 5033 (class 2606 OID 25141)
-- Name: pangya_guild_atividade_player ix_pangya_guild_atividade_player; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_guild_atividade_player
    ADD CONSTRAINT ix_pangya_guild_atividade_player UNIQUE (idx);


--
-- TOC entry 5035 (class 2606 OID 25143)
-- Name: pangya_guild_bbs ix_pangya_guild_bbs; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_guild_bbs
    ADD CONSTRAINT ix_pangya_guild_bbs UNIQUE (seq);


--
-- TOC entry 5039 (class 2606 OID 25145)
-- Name: pangya_guild_bbs_res ix_pangya_guild_bbs_res; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_guild_bbs_res
    ADD CONSTRAINT ix_pangya_guild_bbs_res UNIQUE (seq);


--
-- TOC entry 5044 (class 2606 OID 25147)
-- Name: pangya_guild_intro_img_log ix_pangya_guild_intro_img_log; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_guild_intro_img_log
    ADD CONSTRAINT ix_pangya_guild_intro_img_log UNIQUE (index);


--
-- TOC entry 5050 (class 2606 OID 25149)
-- Name: pangya_guild_match ix_pangya_guild_match; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_guild_match
    ADD CONSTRAINT ix_pangya_guild_match UNIQUE (index);


--
-- TOC entry 5055 (class 2606 OID 25151)
-- Name: pangya_guild_notice ix_pangya_guild_notice; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_guild_notice
    ADD CONSTRAINT ix_pangya_guild_notice UNIQUE (seq);


--
-- TOC entry 5060 (class 2606 OID 25153)
-- Name: pangya_guild_private_bbs ix_pangya_guild_private_bbs; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_guild_private_bbs
    ADD CONSTRAINT ix_pangya_guild_private_bbs UNIQUE (seq);


--
-- TOC entry 5065 (class 2606 OID 25155)
-- Name: pangya_guild_private_bbs_res ix_pangya_guild_private_bbs_res; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_guild_private_bbs_res
    ADD CONSTRAINT ix_pangya_guild_private_bbs_res UNIQUE (seq);


--
-- TOC entry 5079 (class 2606 OID 25157)
-- Name: pangya_item_buff ix_pangya_item_buff; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_item_buff
    ADD CONSTRAINT ix_pangya_item_buff UNIQUE (index);


--
-- TOC entry 5093 (class 2606 OID 25159)
-- Name: pangya_mac_table ix_pangya_mac_table; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_mac_table
    ADD CONSTRAINT ix_pangya_mac_table UNIQUE (index);


--
-- TOC entry 5106 (class 2606 OID 25161)
-- Name: pangya_new_box ix_pangya_new_box; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_new_box
    ADD CONSTRAINT ix_pangya_new_box UNIQUE (id);


--
-- TOC entry 5108 (class 2606 OID 25163)
-- Name: pangya_new_box_item ix_pangya_new_box_item; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_new_box_item
    ADD CONSTRAINT ix_pangya_new_box_item UNIQUE (index);


--
-- TOC entry 5110 (class 2606 OID 25165)
-- Name: pangya_new_box_rare_win_log ix_pangya_new_box_rare_win_log; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_new_box_rare_win_log
    ADD CONSTRAINT ix_pangya_new_box_rare_win_log UNIQUE (index);


--
-- TOC entry 5112 (class 2606 OID 25167)
-- Name: pangya_new_card_pack ix_pangya_new_card_pack; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_new_card_pack
    ADD CONSTRAINT ix_pangya_new_card_pack UNIQUE (index);


--
-- TOC entry 5115 (class 2606 OID 25169)
-- Name: pangya_new_cards ix_pangya_new_cards; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_new_cards
    ADD CONSTRAINT ix_pangya_new_cards UNIQUE (index);


--
-- TOC entry 5119 (class 2606 OID 25171)
-- Name: pangya_new_course_drop_item ix_pangya_new_course_drop; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_new_course_drop_item
    ADD CONSTRAINT ix_pangya_new_course_drop UNIQUE (index);


--
-- TOC entry 5117 (class 2606 OID 25173)
-- Name: pangya_new_course_drop ix_pangya_new_course_drop_1; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_new_course_drop
    ADD CONSTRAINT ix_pangya_new_course_drop_1 UNIQUE (index);


--
-- TOC entry 5121 (class 2606 OID 25175)
-- Name: pangya_new_memorial_level ix_pangya_new_memorial_level; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_new_memorial_level
    ADD CONSTRAINT ix_pangya_new_memorial_level UNIQUE (level);


--
-- TOC entry 5123 (class 2606 OID 25177)
-- Name: pangya_new_memorial_lucky_set ix_pangya_new_memorial_lucky_set; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_new_memorial_lucky_set
    ADD CONSTRAINT ix_pangya_new_memorial_lucky_set UNIQUE (index);


--
-- TOC entry 5125 (class 2606 OID 25179)
-- Name: pangya_new_memorial_normal_item ix_pangya_new_memorial_normal_item; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_new_memorial_normal_item
    ADD CONSTRAINT ix_pangya_new_memorial_normal_item UNIQUE (index);


--
-- TOC entry 5127 (class 2606 OID 25181)
-- Name: pangya_new_memorial_rare_win_log ix_pangya_new_memorial_rare_win_log; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_new_memorial_rare_win_log
    ADD CONSTRAINT ix_pangya_new_memorial_rare_win_log UNIQUE (index);


--
-- TOC entry 5047 (class 2606 OID 25183)
-- Name: pangya_guild_mark_log ix_pangya_pangya_guild_mark_log; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_guild_mark_log
    ADD CONSTRAINT ix_pangya_pangya_guild_mark_log UNIQUE (index);


--
-- TOC entry 5131 (class 2606 OID 25185)
-- Name: pangya_papel_shop_coupon ix_pangya_papel_shop_coupon; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_papel_shop_coupon
    ADD CONSTRAINT ix_pangya_papel_shop_coupon UNIQUE (index);


--
-- TOC entry 5135 (class 2606 OID 25187)
-- Name: pangya_papel_shop_info ix_pangya_papel_shop_info; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_papel_shop_info
    ADD CONSTRAINT ix_pangya_papel_shop_info UNIQUE (index);


--
-- TOC entry 5137 (class 2606 OID 25189)
-- Name: pangya_papel_shop_item ix_pangya_papel_shop_item; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_papel_shop_item
    ADD CONSTRAINT ix_pangya_papel_shop_item UNIQUE (index);


--
-- TOC entry 5139 (class 2606 OID 25191)
-- Name: pangya_papel_shop_rare_win_log ix_pangya_papel_shop_rare_win; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_papel_shop_rare_win_log
    ADD CONSTRAINT ix_pangya_papel_shop_rare_win UNIQUE (index);


--
-- TOC entry 5143 (class 2606 OID 25193)
-- Name: pangya_personal_shop_log ix_pangya_personal_shop_log; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_personal_shop_log
    ADD CONSTRAINT ix_pangya_personal_shop_log UNIQUE (index);


--
-- TOC entry 5148 (class 2606 OID 25195)
-- Name: pangya_player_location ix_pangya_player_location; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_player_location
    ADD CONSTRAINT ix_pangya_player_location UNIQUE (uid);


--
-- TOC entry 5166 (class 2606 OID 25197)
-- Name: pangya_rescue_pwd_log ix_pangya_rescue_pwd_log_2; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_rescue_pwd_log
    ADD CONSTRAINT ix_pangya_rescue_pwd_log_2 UNIQUE (index);


--
-- TOC entry 5171 (class 2606 OID 25199)
-- Name: pangya_server_list ix_pangya_server_list; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_server_list
    ADD CONSTRAINT ix_pangya_server_list UNIQUE (uid);


--
-- TOC entry 5133 (class 2606 OID 25201)
-- Name: pangya_papel_shop_coupon pangya_papel_shop_coupon_pkey; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_papel_shop_coupon
    ADD CONSTRAINT pangya_papel_shop_coupon_pkey PRIMARY KEY (index);


--
-- TOC entry 4898 (class 2606 OID 25203)
-- Name: account pk_account_uid; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.account
    ADD CONSTRAINT pk_account_uid PRIMARY KEY (uid);


--
-- TOC entry 4900 (class 2606 OID 25205)
-- Name: achievement_quest pk_achievement_quest_idx; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.achievement_quest
    ADD CONSTRAINT pk_achievement_quest_idx PRIMARY KEY (idx);


--
-- TOC entry 4902 (class 2606 OID 25207)
-- Name: achievement_tipo pk_achievement_tipo_id_achievement; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.achievement_tipo
    ADD CONSTRAINT pk_achievement_tipo_id_achievement PRIMARY KEY (id_achievement);


--
-- TOC entry 4904 (class 2606 OID 25209)
-- Name: black_papel_item pk_black_papel_item_index; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.black_papel_item
    ADD CONSTRAINT pk_black_papel_item_index PRIMARY KEY (index);


--
-- TOC entry 4906 (class 2606 OID 25211)
-- Name: black_papel_prob_sec pk_black_papel_prob_sec_uid; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.black_papel_prob_sec
    ADD CONSTRAINT pk_black_papel_prob_sec_uid PRIMARY KEY (uid);


--
-- TOC entry 4908 (class 2606 OID 25213)
-- Name: contas_beta pk_contas_beta_index; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.contas_beta
    ADD CONSTRAINT pk_contas_beta_index PRIMARY KEY (index);


--
-- TOC entry 4910 (class 2606 OID 25215)
-- Name: count_item pk_count_item_count_id; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.count_item
    ADD CONSTRAINT pk_count_item_count_id PRIMARY KEY (count_id);


--
-- TOC entry 4912 (class 2606 OID 25217)
-- Name: pangya_1st_anniversary pk_pangya_1st_aniversary; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_1st_anniversary
    ADD CONSTRAINT pk_pangya_1st_aniversary PRIMARY KEY (event_done);


--
-- TOC entry 4916 (class 2606 OID 25219)
-- Name: pangya_1st_anniversary_player_win_cp pk_pangya_1st_aniversary_player_win_cp; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_1st_anniversary_player_win_cp
    ADD CONSTRAINT pk_pangya_1st_aniversary_player_win_cp PRIMARY KEY (index);


--
-- TOC entry 4920 (class 2606 OID 25221)
-- Name: pangya_achievement pk_pangya_achievement; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_achievement
    ADD CONSTRAINT pk_pangya_achievement PRIMARY KEY (id_achievement);


--
-- TOC entry 4922 (class 2606 OID 25223)
-- Name: pangya_approach_missions pk_pangya_approach_missions_numero; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_approach_missions
    ADD CONSTRAINT pk_pangya_approach_missions_numero PRIMARY KEY (numero);


--
-- TOC entry 4924 (class 2606 OID 25225)
-- Name: pangya_assistente pk_pangya_assistente_uid; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_assistente
    ADD CONSTRAINT pk_pangya_assistente_uid PRIMARY KEY (uid);


--
-- TOC entry 4926 (class 2606 OID 25227)
-- Name: pangya_attendance_reward pk_pangya_attendance_reward_uid; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_attendance_reward
    ADD CONSTRAINT pk_pangya_attendance_reward_uid PRIMARY KEY (uid);


--
-- TOC entry 4928 (class 2606 OID 25229)
-- Name: pangya_attendance_table_item_reward pk_pangya_attendance_table_item_reward_idx; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_attendance_table_item_reward
    ADD CONSTRAINT pk_pangya_attendance_table_item_reward_idx PRIMARY KEY (idx);


--
-- TOC entry 4932 (class 2606 OID 25231)
-- Name: pangya_bot_gm_event_reward pk_pangya_bot_gm_event_reward; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_bot_gm_event_reward
    ADD CONSTRAINT pk_pangya_bot_gm_event_reward PRIMARY KEY (index);


--
-- TOC entry 4934 (class 2606 OID 25233)
-- Name: pangya_bot_gm_event_time pk_pangya_bot_gm_event_time; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_bot_gm_event_time
    ADD CONSTRAINT pk_pangya_bot_gm_event_time PRIMARY KEY (index);


--
-- TOC entry 4938 (class 2606 OID 25235)
-- Name: pangya_caddie_information pk_pangya_caddie_information_item_id; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_caddie_information
    ADD CONSTRAINT pk_pangya_caddie_information_item_id PRIMARY KEY (item_id);


--
-- TOC entry 4940 (class 2606 OID 25237)
-- Name: pangya_card pk_pangya_card_card_itemid; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_card
    ADD CONSTRAINT pk_pangya_card_card_itemid PRIMARY KEY (card_itemid);


--
-- TOC entry 4954 (class 2606 OID 25239)
-- Name: pangya_coin_cube_location pk_pangya_coin_cube; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_coin_cube_location
    ADD CONSTRAINT pk_pangya_coin_cube PRIMARY KEY (index);


--
-- TOC entry 4958 (class 2606 OID 25241)
-- Name: pangya_command pk_pangya_command_idx; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_command
    ADD CONSTRAINT pk_pangya_command_idx PRIMARY KEY (idx);


--
-- TOC entry 4972 (class 2606 OID 25243)
-- Name: pangya_counter_item pk_pangya_counter_item; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_counter_item
    ADD CONSTRAINT pk_pangya_counter_item PRIMARY KEY (count_id);


--
-- TOC entry 4974 (class 2606 OID 25245)
-- Name: pangya_course_cube_coin_temporada pk_pangya_course_cube_coin_temporada_index; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_course_cube_coin_temporada
    ADD CONSTRAINT pk_pangya_course_cube_coin_temporada_index PRIMARY KEY (index);


--
-- TOC entry 4976 (class 2606 OID 25247)
-- Name: pangya_course_drop pk_pangya_course_drop_index; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_course_drop
    ADD CONSTRAINT pk_pangya_course_drop_index PRIMARY KEY (index);


--
-- TOC entry 4978 (class 2606 OID 25249)
-- Name: pangya_course_drop_item pk_pangya_course_drop_item_index; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_course_drop_item
    ADD CONSTRAINT pk_pangya_course_drop_item_index PRIMARY KEY (index);


--
-- TOC entry 4980 (class 2606 OID 25251)
-- Name: pangya_course_reward_treasure pk_pangya_course_reward_treasure_course; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_course_reward_treasure
    ADD CONSTRAINT pk_pangya_course_reward_treasure_course PRIMARY KEY (course);


--
-- TOC entry 4982 (class 2606 OID 25253)
-- Name: pangya_cube_coin_location pk_pangya_cube_coin_location_index; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_cube_coin_location
    ADD CONSTRAINT pk_pangya_cube_coin_location_index PRIMARY KEY (index);


--
-- TOC entry 4984 (class 2606 OID 25255)
-- Name: pangya_daily_quest_player pk_pangya_daily_quest_player_uid; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_daily_quest_player
    ADD CONSTRAINT pk_pangya_daily_quest_player_uid PRIMARY KEY (uid);


--
-- TOC entry 4988 (class 2606 OID 25257)
-- Name: pangya_dolfini_locker_item pk_pangya_dolfini_locker_item_idx; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_dolfini_locker_item
    ADD CONSTRAINT pk_pangya_dolfini_locker_item_idx PRIMARY KEY (idx);


--
-- TOC entry 4986 (class 2606 OID 25259)
-- Name: pangya_dolfini_locker pk_pangya_dolfini_locker_uid; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_dolfini_locker
    ADD CONSTRAINT pk_pangya_dolfini_locker_uid PRIMARY KEY (uid);


--
-- TOC entry 4994 (class 2606 OID 25261)
-- Name: pangya_donation_epin pk_pangya_donation_epin; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_donation_epin
    ADD CONSTRAINT pk_pangya_donation_epin PRIMARY KEY (index);


--
-- TOC entry 5004 (class 2606 OID 25263)
-- Name: pangya_donation_new pk_pangya_donation_new; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_donation_new
    ADD CONSTRAINT pk_pangya_donation_new PRIMARY KEY (index);


--
-- TOC entry 5006 (class 2606 OID 25265)
-- Name: pangya_gacha_jp_all_item_list pk_pangya_gacha_jp_all_item_list; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_gacha_jp_all_item_list
    ADD CONSTRAINT pk_pangya_gacha_jp_all_item_list PRIMARY KEY (index);


--
-- TOC entry 5012 (class 2606 OID 25267)
-- Name: pangya_gift_table pk_pangya_gift_table_msg_id; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_gift_table
    ADD CONSTRAINT pk_pangya_gift_table_msg_id PRIMARY KEY (msg_id);


--
-- TOC entry 5017 (class 2606 OID 25269)
-- Name: pangya_golden_time_info pk_pangya_golden_time_info; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_golden_time_info
    ADD CONSTRAINT pk_pangya_golden_time_info PRIMARY KEY (index);


--
-- TOC entry 5019 (class 2606 OID 25271)
-- Name: pangya_golden_time_item pk_pangya_golden_time_item; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_golden_time_item
    ADD CONSTRAINT pk_pangya_golden_time_item PRIMARY KEY (index);


--
-- TOC entry 5021 (class 2606 OID 25273)
-- Name: pangya_golden_time_round pk_pangya_golden_time_round; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_golden_time_round
    ADD CONSTRAINT pk_pangya_golden_time_round PRIMARY KEY (index);


--
-- TOC entry 5023 (class 2606 OID 25275)
-- Name: pangya_grand_zodiac_pontos pk_pangya_grand_zodiac_pontos_uid; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_grand_zodiac_pontos
    ADD CONSTRAINT pk_pangya_grand_zodiac_pontos_uid PRIMARY KEY (uid);


--
-- TOC entry 5025 (class 2606 OID 25277)
-- Name: pangya_grand_zodiac_times pk_pangya_grand_zodiac_times_index; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_grand_zodiac_times
    ADD CONSTRAINT pk_pangya_grand_zodiac_times_index PRIMARY KEY (index);


--
-- TOC entry 5027 (class 2606 OID 25279)
-- Name: pangya_grandprix_clear pk_pangya_grandprix_clear_index; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_grandprix_clear
    ADD CONSTRAINT pk_pangya_grandprix_clear_index PRIMARY KEY (index);


--
-- TOC entry 5029 (class 2606 OID 25281)
-- Name: pangya_grandprix_event_config pk_pangya_grandprix_event_config_index; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_grandprix_event_config
    ADD CONSTRAINT pk_pangya_grandprix_event_config_index PRIMARY KEY (index);


--
-- TOC entry 5073 (class 2606 OID 25283)
-- Name: pangya_guild_update_activity pk_pangya_guild_update_activity; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_guild_update_activity
    ADD CONSTRAINT pk_pangya_guild_update_activity PRIMARY KEY (index);


--
-- TOC entry 5075 (class 2606 OID 25285)
-- Name: pangya_gz_event_2016121600_rare_win pk_pangya_gz_event_2016121600_rare_win_index; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_gz_event_2016121600_rare_win
    ADD CONSTRAINT pk_pangya_gz_event_2016121600_rare_win_index PRIMARY KEY (index);


--
-- TOC entry 5077 (class 2606 OID 25287)
-- Name: pangya_ip_table pk_pangya_ip_table_index; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_ip_table
    ADD CONSTRAINT pk_pangya_ip_table_index PRIMARY KEY (index);


--
-- TOC entry 5083 (class 2606 OID 25289)
-- Name: pangya_item_warehouse pk_pangya_item_warehouse_item_id; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_item_warehouse
    ADD CONSTRAINT pk_pangya_item_warehouse_item_id PRIMARY KEY (item_id);


--
-- TOC entry 5085 (class 2606 OID 25291)
-- Name: pangya_login_reward pk_pangya_login_reward; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_login_reward
    ADD CONSTRAINT pk_pangya_login_reward PRIMARY KEY (index);


--
-- TOC entry 5087 (class 2606 OID 25293)
-- Name: pangya_login_reward_player pk_pangya_login_reward_player; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_login_reward_player
    ADD CONSTRAINT pk_pangya_login_reward_player PRIMARY KEY (index);


--
-- TOC entry 5089 (class 2606 OID 25295)
-- Name: pangya_lucia_attendance pk_pangya_lucia_attendance; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_lucia_attendance
    ADD CONSTRAINT pk_pangya_lucia_attendance PRIMARY KEY (uid);


--
-- TOC entry 5098 (class 2606 OID 25297)
-- Name: pangya_mascot_info pk_pangya_mascot_info_item_id; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_mascot_info
    ADD CONSTRAINT pk_pangya_mascot_info_item_id PRIMARY KEY (item_id);


--
-- TOC entry 5100 (class 2606 OID 25299)
-- Name: pangya_memorial_shop_level pk_pangya_memorial_shop_level_level; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_memorial_shop_level
    ADD CONSTRAINT pk_pangya_memorial_shop_level_level PRIMARY KEY (level);


--
-- TOC entry 5102 (class 2606 OID 25301)
-- Name: pangya_msg_user pk_pangya_msg_user_msg_idx; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_msg_user
    ADD CONSTRAINT pk_pangya_msg_user_msg_idx PRIMARY KEY (msg_idx);


--
-- TOC entry 5104 (class 2606 OID 25303)
-- Name: pangya_myroom pk_pangya_myroom_uid; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_myroom
    ADD CONSTRAINT pk_pangya_myroom_uid PRIMARY KEY (uid);


--
-- TOC entry 5129 (class 2606 OID 25305)
-- Name: pangya_notice_list pk_pangya_notice_list_notice_id; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_notice_list
    ADD CONSTRAINT pk_pangya_notice_list_notice_id PRIMARY KEY (notice_id);


--
-- TOC entry 4952 (class 2606 OID 25307)
-- Name: pangya_character_information pk_pangya_pangya_character_information; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_character_information
    ADD CONSTRAINT pk_pangya_pangya_character_information PRIMARY KEY (item_id);


--
-- TOC entry 5008 (class 2606 OID 25309)
-- Name: pangya_gacha_jp_item_list pk_pangya_pangya_gacha_jp_item_list; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_gacha_jp_item_list
    ADD CONSTRAINT pk_pangya_pangya_gacha_jp_item_list PRIMARY KEY (index);


--
-- TOC entry 5010 (class 2606 OID 25311)
-- Name: pangya_gacha_jp_player_win pk_pangya_pangya_gacha_jp_player_win; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_gacha_jp_player_win
    ADD CONSTRAINT pk_pangya_pangya_gacha_jp_player_win PRIMARY KEY (index);


--
-- TOC entry 5070 (class 2606 OID 25313)
-- Name: pangya_guild_ranking pk_pangya_pangya_guild_ranking; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_guild_ranking
    ADD CONSTRAINT pk_pangya_pangya_guild_ranking PRIMARY KEY (index);


--
-- TOC entry 5091 (class 2606 OID 25315)
-- Name: pangya_lucia_attendance_reward_log pk_pangya_pangya_lucia_attendance_log; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_lucia_attendance_reward_log
    ADD CONSTRAINT pk_pangya_pangya_lucia_attendance_log PRIMARY KEY (index);


--
-- TOC entry 5141 (class 2606 OID 25317)
-- Name: pangya_parts_list pk_pangya_parts_list_index; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_parts_list
    ADD CONSTRAINT pk_pangya_parts_list_index PRIMARY KEY (index);


--
-- TOC entry 5146 (class 2606 OID 25319)
-- Name: pangya_player_ip pk_pangya_player_ip_index; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_player_ip
    ADD CONSTRAINT pk_pangya_player_ip_index PRIMARY KEY (index);


--
-- TOC entry 5153 (class 2606 OID 25321)
-- Name: pangya_quest pk_pangya_quest; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_quest
    ADD CONSTRAINT pk_pangya_quest PRIMARY KEY (id);


--
-- TOC entry 5155 (class 2606 OID 25323)
-- Name: pangya_quest_clear pk_pangya_quest_clear_index; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_quest_clear
    ADD CONSTRAINT pk_pangya_quest_clear_index PRIMARY KEY (index);


--
-- TOC entry 5157 (class 2606 OID 25325)
-- Name: pangya_rank_antes pk_pangya_rank_antes_index; Type: CONSTRAINT; Schema: pangya; Owner: pangya
--

ALTER TABLE ONLY pangya.pangya_rank_antes
    ADD CONSTRAINT pk_pangya_rank_antes_index PRIMARY KEY (index);


--
-- TOC entry 5161 (class 2606 OID 25327)
-- Name: pangya_rank_atual_character pk_pangya_rank_atual_character_uid; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_rank_atual_character
    ADD CONSTRAINT pk_pangya_rank_atual_character_uid PRIMARY KEY (uid);


--
-- TOC entry 5159 (class 2606 OID 25329)
-- Name: pangya_rank_atual pk_pangya_rank_atual_index; Type: CONSTRAINT; Schema: pangya; Owner: pangya
--

ALTER TABLE ONLY pangya.pangya_rank_atual
    ADD CONSTRAINT pk_pangya_rank_atual_index PRIMARY KEY (index);


--
-- TOC entry 5163 (class 2606 OID 25331)
-- Name: pangya_rank_config pk_pangya_rank_config_index; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_rank_config
    ADD CONSTRAINT pk_pangya_rank_config_index PRIMARY KEY (index);


--
-- TOC entry 5169 (class 2606 OID 25333)
-- Name: pangya_scratchy_prob_sec pk_pangya_scratchy_prob_sec_uid; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_scratchy_prob_sec
    ADD CONSTRAINT pk_pangya_scratchy_prob_sec_uid PRIMARY KEY (uid);


--
-- TOC entry 5173 (class 2606 OID 25335)
-- Name: pangya_shutdown_list pk_pangya_shutdown_list_id; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_shutdown_list
    ADD CONSTRAINT pk_pangya_shutdown_list_id PRIMARY KEY (id);


--
-- TOC entry 5175 (class 2606 OID 25337)
-- Name: pangya_ticker_list pk_pangya_ticker_list_ticker_id; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_ticker_list
    ADD CONSTRAINT pk_pangya_ticker_list_ticker_id PRIMARY KEY (ticker_id);


--
-- TOC entry 5177 (class 2606 OID 25340)
-- Name: pangya_ticket_report pk_pangya_ticket_report_idx; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_ticket_report
    ADD CONSTRAINT pk_pangya_ticket_report_idx PRIMARY KEY (idx);


--
-- TOC entry 5179 (class 2606 OID 25342)
-- Name: pangya_transforme_clubset_temp pk_pangya_transforme_clubset_temp_trans_index; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_transforme_clubset_temp
    ADD CONSTRAINT pk_pangya_transforme_clubset_temp_trans_index PRIMARY KEY (trans_index);


--
-- TOC entry 5181 (class 2606 OID 25344)
-- Name: pangya_trofel_especial pk_pangya_trofel_especial_item_id; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_trofel_especial
    ADD CONSTRAINT pk_pangya_trofel_especial_item_id PRIMARY KEY (item_id);


--
-- TOC entry 5183 (class 2606 OID 25346)
-- Name: pangya_trofel_grandprix pk_pangya_trofel_grandprix_item_id; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_trofel_grandprix
    ADD CONSTRAINT pk_pangya_trofel_grandprix_item_id PRIMARY KEY (item_id);


--
-- TOC entry 5185 (class 2606 OID 25348)
-- Name: pangya_user_equip pk_pangya_user_equip_uid; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_user_equip
    ADD CONSTRAINT pk_pangya_user_equip_uid PRIMARY KEY (uid);


--
-- TOC entry 5187 (class 2606 OID 25350)
-- Name: pangya_weblink_key pk_pangya_weblink_key_index; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.pangya_weblink_key
    ADD CONSTRAINT pk_pangya_weblink_key_index PRIMARY KEY (index);


--
-- TOC entry 5189 (class 2606 OID 25352)
-- Name: td_char_equip_s4 pk_td_char_equip_s4_seq; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.td_char_equip_s4
    ADD CONSTRAINT pk_td_char_equip_s4_seq PRIMARY KEY (seq);


--
-- TOC entry 5191 (class 2606 OID 25354)
-- Name: td_room_data pk_td_room_data_myroom_id; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.td_room_data
    ADD CONSTRAINT pk_td_room_data_myroom_id PRIMARY KEY (myroom_id);


--
-- TOC entry 5193 (class 2606 OID 25356)
-- Name: trofel_stat pk_trofel_stat_uid; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.trofel_stat
    ADD CONSTRAINT pk_trofel_stat_uid PRIMARY KEY (uid);


--
-- TOC entry 5195 (class 2606 OID 25358)
-- Name: tu_ucc pk_tu_ucc_uid; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.tu_ucc
    ADD CONSTRAINT pk_tu_ucc_uid PRIMARY KEY (uid, typeid, seq, item_id);


--
-- TOC entry 5197 (class 2606 OID 25360)
-- Name: tutorial pk_tutorial_uid; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.tutorial
    ADD CONSTRAINT pk_tutorial_uid PRIMARY KEY (uid);


--
-- TOC entry 5199 (class 2606 OID 25362)
-- Name: user_info pk_user_info_uid; Type: CONSTRAINT; Schema: pangya; Owner: postgres
--

ALTER TABLE ONLY pangya.user_info
    ADD CONSTRAINT pk_user_info_uid PRIMARY KEY (uid);


--
-- TOC entry 4896 (class 1259 OID 25363)
-- Name: ix_account_1; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE UNIQUE INDEX ix_account_1 ON pangya.account USING btree (uid);


--
-- TOC entry 4917 (class 1259 OID 25364)
-- Name: ix_pangya_achievement_1; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE UNIQUE INDEX ix_pangya_achievement_1 ON pangya.pangya_achievement USING btree (id_achievement);


--
-- TOC entry 4918 (class 1259 OID 25365)
-- Name: ix_pangya_achievement_2; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE INDEX ix_pangya_achievement_2 ON pangya.pangya_achievement USING btree (uid);


--
-- TOC entry 4935 (class 1259 OID 25366)
-- Name: ix_pangya_caddie_information; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE UNIQUE INDEX ix_pangya_caddie_information ON pangya.pangya_caddie_information USING btree (item_id);


--
-- TOC entry 4936 (class 1259 OID 25367)
-- Name: ix_pangya_caddie_information_1; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE INDEX ix_pangya_caddie_information_1 ON pangya.pangya_caddie_information USING btree (uid);


--
-- TOC entry 4945 (class 1259 OID 25368)
-- Name: ix_pangya_change_nickname_log_1; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE UNIQUE INDEX ix_pangya_change_nickname_log_1 ON pangya.pangya_change_nickname_log USING btree (index);


--
-- TOC entry 4948 (class 1259 OID 25369)
-- Name: ix_pangya_change_pwd_log_1; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE UNIQUE INDEX ix_pangya_change_pwd_log_1 ON pangya.pangya_change_pwd_log USING btree (uid);


--
-- TOC entry 4949 (class 1259 OID 25370)
-- Name: ix_pangya_character_information; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE UNIQUE INDEX ix_pangya_character_information ON pangya.pangya_character_information USING btree (item_id);


--
-- TOC entry 4950 (class 1259 OID 25373)
-- Name: ix_pangya_character_information_1; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE INDEX ix_pangya_character_information_1 ON pangya.pangya_character_information USING btree (uid);


--
-- TOC entry 4963 (class 1259 OID 25374)
-- Name: ix_pangya_cookie_point_item_log_1; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE UNIQUE INDEX ix_pangya_cookie_point_item_log_1 ON pangya.pangya_cookie_point_item_log USING btree (index);


--
-- TOC entry 4964 (class 1259 OID 25375)
-- Name: ix_pangya_cookie_point_item_log_2; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE INDEX ix_pangya_cookie_point_item_log_2 ON pangya.pangya_cookie_point_item_log USING btree (cp_id_log);


--
-- TOC entry 4967 (class 1259 OID 25376)
-- Name: ix_pangya_cookie_point_log_1; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE UNIQUE INDEX ix_pangya_cookie_point_log_1 ON pangya.pangya_cookie_point_log USING btree (id);


--
-- TOC entry 4968 (class 1259 OID 25378)
-- Name: ix_pangya_cookie_point_log_2; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE INDEX ix_pangya_cookie_point_log_2 ON pangya.pangya_cookie_point_log USING btree (uid);


--
-- TOC entry 4969 (class 1259 OID 25379)
-- Name: ix_pangya_counter_item; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE UNIQUE INDEX ix_pangya_counter_item ON pangya.pangya_counter_item USING btree (count_id);


--
-- TOC entry 4970 (class 1259 OID 25387)
-- Name: ix_pangya_counter_item_1; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE INDEX ix_pangya_counter_item_1 ON pangya.pangya_counter_item USING btree (uid);


--
-- TOC entry 4997 (class 1259 OID 25388)
-- Name: ix_pangya_donation_item_log_1; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE UNIQUE INDEX ix_pangya_donation_item_log_1 ON pangya.pangya_donation_item_log USING btree (index);


--
-- TOC entry 5000 (class 1259 OID 25389)
-- Name: ix_pangya_donation_log_1; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE UNIQUE INDEX ix_pangya_donation_log_1 ON pangya.pangya_donation_log USING btree (index);


--
-- TOC entry 5015 (class 1259 OID 25390)
-- Name: ix_pangya_gm_gift_web_log_1; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE UNIQUE INDEX ix_pangya_gm_gift_web_log_1 ON pangya.pangya_gm_gift_web_log USING btree (index);


--
-- TOC entry 5036 (class 1259 OID 25391)
-- Name: ix_pangya_guild_bbs_1; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE UNIQUE INDEX ix_pangya_guild_bbs_1 ON pangya.pangya_guild_bbs USING btree (seq);


--
-- TOC entry 5037 (class 1259 OID 25392)
-- Name: ix_pangya_guild_bbs_2; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE INDEX ix_pangya_guild_bbs_2 ON pangya.pangya_guild_bbs USING btree (owner_uid);


--
-- TOC entry 5040 (class 1259 OID 25393)
-- Name: ix_pangya_guild_bbs_res_1; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE UNIQUE INDEX ix_pangya_guild_bbs_res_1 ON pangya.pangya_guild_bbs_res USING btree (seq);


--
-- TOC entry 5041 (class 1259 OID 25394)
-- Name: ix_pangya_guild_bbs_res_2; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE INDEX ix_pangya_guild_bbs_res_2 ON pangya.pangya_guild_bbs_res USING btree (bbs_seq);


--
-- TOC entry 5042 (class 1259 OID 25395)
-- Name: ix_pangya_guild_bbs_res_3; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE INDEX ix_pangya_guild_bbs_res_3 ON pangya.pangya_guild_bbs_res USING btree (owner_uid);


--
-- TOC entry 5045 (class 1259 OID 25396)
-- Name: ix_pangya_guild_intro_img_log_1; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE UNIQUE INDEX ix_pangya_guild_intro_img_log_1 ON pangya.pangya_guild_intro_img_log USING btree (index);


--
-- TOC entry 5051 (class 1259 OID 25397)
-- Name: ix_pangya_guild_match_1; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE UNIQUE INDEX ix_pangya_guild_match_1 ON pangya.pangya_guild_match USING btree (index);


--
-- TOC entry 5052 (class 1259 OID 25398)
-- Name: ix_pangya_guild_match_2; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE INDEX ix_pangya_guild_match_2 ON pangya.pangya_guild_match USING btree (guild_1_uid);


--
-- TOC entry 5053 (class 1259 OID 25399)
-- Name: ix_pangya_guild_match_3; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE INDEX ix_pangya_guild_match_3 ON pangya.pangya_guild_match USING btree (guild_2_uid);


--
-- TOC entry 5056 (class 1259 OID 25400)
-- Name: ix_pangya_guild_notice_1; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE UNIQUE INDEX ix_pangya_guild_notice_1 ON pangya.pangya_guild_notice USING btree (seq);


--
-- TOC entry 5057 (class 1259 OID 25402)
-- Name: ix_pangya_guild_notice_2; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE INDEX ix_pangya_guild_notice_2 ON pangya.pangya_guild_notice USING btree (guild_uid);


--
-- TOC entry 5058 (class 1259 OID 25403)
-- Name: ix_pangya_guild_notice_3; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE INDEX ix_pangya_guild_notice_3 ON pangya.pangya_guild_notice USING btree (owner_uid);


--
-- TOC entry 5061 (class 1259 OID 25404)
-- Name: ix_pangya_guild_private_bbs_1; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE UNIQUE INDEX ix_pangya_guild_private_bbs_1 ON pangya.pangya_guild_private_bbs USING btree (seq);


--
-- TOC entry 5062 (class 1259 OID 25405)
-- Name: ix_pangya_guild_private_bbs_2; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE INDEX ix_pangya_guild_private_bbs_2 ON pangya.pangya_guild_private_bbs USING btree (guild_uid);


--
-- TOC entry 5063 (class 1259 OID 25406)
-- Name: ix_pangya_guild_private_bbs_3; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE INDEX ix_pangya_guild_private_bbs_3 ON pangya.pangya_guild_private_bbs USING btree (owner_uid);


--
-- TOC entry 5066 (class 1259 OID 25407)
-- Name: ix_pangya_guild_private_bbs_res_1; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE UNIQUE INDEX ix_pangya_guild_private_bbs_res_1 ON pangya.pangya_guild_private_bbs_res USING btree (seq);


--
-- TOC entry 5067 (class 1259 OID 25408)
-- Name: ix_pangya_guild_private_bbs_res_2; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE INDEX ix_pangya_guild_private_bbs_res_2 ON pangya.pangya_guild_private_bbs_res USING btree (guild_bbs_seq);


--
-- TOC entry 5068 (class 1259 OID 25409)
-- Name: ix_pangya_guild_private_bbs_res_3; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE INDEX ix_pangya_guild_private_bbs_res_3 ON pangya.pangya_guild_private_bbs_res USING btree (owner_uid);


--
-- TOC entry 5071 (class 1259 OID 25410)
-- Name: ix_pangya_guild_update_activity; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE INDEX ix_pangya_guild_update_activity ON pangya.pangya_guild_update_activity USING btree (owner_update);


--
-- TOC entry 5080 (class 1259 OID 25411)
-- Name: ix_pangya_item_warehouse; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE UNIQUE INDEX ix_pangya_item_warehouse ON pangya.pangya_item_warehouse USING btree (item_id);


--
-- TOC entry 5081 (class 1259 OID 25412)
-- Name: ix_pangya_item_warehouse_1; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE INDEX ix_pangya_item_warehouse_1 ON pangya.pangya_item_warehouse USING btree (uid);


--
-- TOC entry 5094 (class 1259 OID 25413)
-- Name: ix_pangya_mac_table_1; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE UNIQUE INDEX ix_pangya_mac_table_1 ON pangya.pangya_mac_table USING btree (index);


--
-- TOC entry 5095 (class 1259 OID 25414)
-- Name: ix_pangya_mascot_info; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE UNIQUE INDEX ix_pangya_mascot_info ON pangya.pangya_mascot_info USING btree (item_id);


--
-- TOC entry 5096 (class 1259 OID 25415)
-- Name: ix_pangya_mascot_info_1; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE INDEX ix_pangya_mascot_info_1 ON pangya.pangya_mascot_info USING btree (uid);


--
-- TOC entry 5113 (class 1259 OID 25416)
-- Name: ix_pangya_new_card_pack_1; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE INDEX ix_pangya_new_card_pack_1 ON pangya.pangya_new_card_pack USING btree (index);


--
-- TOC entry 5048 (class 1259 OID 25417)
-- Name: ix_pangya_pangya_guild_mark_log_1; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE UNIQUE INDEX ix_pangya_pangya_guild_mark_log_1 ON pangya.pangya_guild_mark_log USING btree (index);


--
-- TOC entry 5144 (class 1259 OID 25418)
-- Name: ix_pangya_personal_shop_log_1; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE UNIQUE INDEX ix_pangya_personal_shop_log_1 ON pangya.pangya_personal_shop_log USING btree (index);


--
-- TOC entry 5149 (class 1259 OID 25419)
-- Name: ix_pangya_quest; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE INDEX ix_pangya_quest ON pangya.pangya_quest USING btree (achievement_id);


--
-- TOC entry 5150 (class 1259 OID 25420)
-- Name: ix_pangya_quest_2; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE UNIQUE INDEX ix_pangya_quest_2 ON pangya.pangya_quest USING btree (id);


--
-- TOC entry 5151 (class 1259 OID 25421)
-- Name: ix_pangya_quest_3; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE INDEX ix_pangya_quest_3 ON pangya.pangya_quest USING btree (uid);


--
-- TOC entry 5164 (class 1259 OID 25422)
-- Name: ix_pangya_rescue_pwd_log; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE INDEX ix_pangya_rescue_pwd_log ON pangya.pangya_rescue_pwd_log USING btree (uid);


--
-- TOC entry 5167 (class 1259 OID 25423)
-- Name: ix_pangya_rescue_pwd_log_3; Type: INDEX; Schema: pangya; Owner: postgres
--

CREATE UNIQUE INDEX ix_pangya_rescue_pwd_log_3 ON pangya.pangya_rescue_pwd_log USING btree (index);


--
-- TOC entry 5335 (class 0 OID 0)
-- Dependencies: 12
-- Name: SCHEMA pangya; Type: ACL; Schema: -; Owner: postgres
--

REVOKE ALL ON SCHEMA pangya FROM postgres;
GRANT ALL ON SCHEMA pangya TO pangya;
GRANT ALL ON SCHEMA pangya TO "pangyaPHP";
GRANT USAGE ON SCHEMA pangya TO postgres;


--
-- TOC entry 5336 (class 0 OID 0)
-- Dependencies: 209
-- Name: TABLE account; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.account TO pangya;
GRANT SELECT,INSERT,UPDATE ON TABLE pangya.account TO "pangyaPHP";


--
-- TOC entry 5337 (class 0 OID 0)
-- Dependencies: 215
-- Name: TABLE authkey_game; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.authkey_game TO pangya;


--
-- TOC entry 5338 (class 0 OID 0)
-- Dependencies: 216
-- Name: TABLE authkey_login; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.authkey_login TO pangya;


--
-- TOC entry 5339 (class 0 OID 0)
-- Dependencies: 217
-- Name: TABLE black_papel_item; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.black_papel_item TO pangya;


--
-- TOC entry 5340 (class 0 OID 0)
-- Dependencies: 219
-- Name: TABLE black_papel_prob_sec; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.black_papel_prob_sec TO pangya;
GRANT INSERT ON TABLE pangya.black_papel_prob_sec TO "pangyaPHP";


--
-- TOC entry 5341 (class 0 OID 0)
-- Dependencies: 220
-- Name: TABLE black_papel_qntd_prob; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.black_papel_qntd_prob TO pangya;


--
-- TOC entry 5342 (class 0 OID 0)
-- Dependencies: 221
-- Name: TABLE black_papel_rate; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.black_papel_rate TO pangya;


--
-- TOC entry 5343 (class 0 OID 0)
-- Dependencies: 222
-- Name: TABLE contas_beta; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.contas_beta TO pangya;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE pangya.contas_beta TO "pangyaPHP";


--
-- TOC entry 5344 (class 0 OID 0)
-- Dependencies: 226
-- Name: TABLE pangya_1st_anniversary; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_1st_anniversary TO pangya;


--
-- TOC entry 5345 (class 0 OID 0)
-- Dependencies: 227
-- Name: TABLE pangya_1st_anniversary_player_win_cp; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_1st_anniversary_player_win_cp TO pangya;


--
-- TOC entry 5346 (class 0 OID 0)
-- Dependencies: 229
-- Name: TABLE pangya_achievement; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_achievement TO pangya;


--
-- TOC entry 5347 (class 0 OID 0)
-- Dependencies: 231
-- Name: TABLE pangya_approach_missions; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_approach_missions TO pangya;


--
-- TOC entry 5348 (class 0 OID 0)
-- Dependencies: 232
-- Name: TABLE pangya_assistente; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_assistente TO pangya;
GRANT INSERT ON TABLE pangya.pangya_assistente TO "pangyaPHP";


--
-- TOC entry 5349 (class 0 OID 0)
-- Dependencies: 233
-- Name: TABLE pangya_attendance_reward; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_attendance_reward TO pangya;
GRANT ALL ON TABLE pangya.pangya_attendance_reward TO "pangyaPHP";


--
-- TOC entry 5350 (class 0 OID 0)
-- Dependencies: 234
-- Name: TABLE pangya_attendance_table_item_reward; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_attendance_table_item_reward TO pangya;


--
-- TOC entry 5351 (class 0 OID 0)
-- Dependencies: 236
-- Name: TABLE pangya_auth_key; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_auth_key TO pangya;


--
-- TOC entry 5352 (class 0 OID 0)
-- Dependencies: 238
-- Name: TABLE pangya_bot_gm_event_reward; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_bot_gm_event_reward TO pangya;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE pangya.pangya_bot_gm_event_reward TO "pangyaPHP";


--
-- TOC entry 5353 (class 0 OID 0)
-- Dependencies: 240
-- Name: TABLE pangya_bot_gm_event_time; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_bot_gm_event_time TO pangya;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE pangya.pangya_bot_gm_event_time TO "pangyaPHP";


--
-- TOC entry 5354 (class 0 OID 0)
-- Dependencies: 242
-- Name: TABLE pangya_caddie_information; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_caddie_information TO pangya;
GRANT SELECT ON TABLE pangya.pangya_caddie_information TO "pangyaPHP";


--
-- TOC entry 5355 (class 0 OID 0)
-- Dependencies: 244
-- Name: TABLE pangya_card; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_card TO pangya;


--
-- TOC entry 5356 (class 0 OID 0)
-- Dependencies: 246
-- Name: TABLE pangya_card_equip; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_card_equip TO pangya;


--
-- TOC entry 5357 (class 0 OID 0)
-- Dependencies: 248
-- Name: TABLE pangya_card_rate; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_card_rate TO pangya;


--
-- TOC entry 5358 (class 0 OID 0)
-- Dependencies: 249
-- Name: TABLE pangya_cardpack; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_cardpack TO pangya;


--
-- TOC entry 5359 (class 0 OID 0)
-- Dependencies: 250
-- Name: TABLE pangya_cards; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_cards TO pangya;


--
-- TOC entry 5360 (class 0 OID 0)
-- Dependencies: 251
-- Name: TABLE pangya_change_nickname_log; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_change_nickname_log TO pangya;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE pangya.pangya_change_nickname_log TO "pangyaPHP";


--
-- TOC entry 5361 (class 0 OID 0)
-- Dependencies: 253
-- Name: TABLE pangya_change_pwd_log; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_change_pwd_log TO pangya;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE pangya.pangya_change_pwd_log TO "pangyaPHP";


--
-- TOC entry 5362 (class 0 OID 0)
-- Dependencies: 254
-- Name: TABLE pangya_character_information; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_character_information TO pangya;


--
-- TOC entry 5363 (class 0 OID 0)
-- Dependencies: 256
-- Name: TABLE pangya_clubset_enchant; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_clubset_enchant TO pangya;


--
-- TOC entry 5364 (class 0 OID 0)
-- Dependencies: 257
-- Name: TABLE pangya_coin_cube_info; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_coin_cube_info TO pangya;


--
-- TOC entry 5365 (class 0 OID 0)
-- Dependencies: 258
-- Name: TABLE pangya_coin_cube_location; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_coin_cube_location TO pangya;


--
-- TOC entry 5366 (class 0 OID 0)
-- Dependencies: 260
-- Name: TABLE pangya_comet_refill; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_comet_refill TO pangya;


--
-- TOC entry 5367 (class 0 OID 0)
-- Dependencies: 262
-- Name: TABLE pangya_command; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_command TO pangya;
GRANT ALL ON TABLE pangya.pangya_command TO "pangyaPHP";


--
-- TOC entry 5368 (class 0 OID 0)
-- Dependencies: 264
-- Name: TABLE pangya_config; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_config TO pangya;
GRANT SELECT ON TABLE pangya.pangya_config TO "pangyaPHP";


--
-- TOC entry 5369 (class 0 OID 0)
-- Dependencies: 265
-- Name: TABLE pangya_cookie_point_item_log; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_cookie_point_item_log TO pangya;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE pangya.pangya_cookie_point_item_log TO "pangyaPHP";


--
-- TOC entry 5370 (class 0 OID 0)
-- Dependencies: 267
-- Name: TABLE pangya_cookie_point_log; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_cookie_point_log TO pangya;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE pangya.pangya_cookie_point_log TO "pangyaPHP";


--
-- TOC entry 5371 (class 0 OID 0)
-- Dependencies: 269
-- Name: TABLE pangya_counter_item; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_counter_item TO pangya;


--
-- TOC entry 5372 (class 0 OID 0)
-- Dependencies: 271
-- Name: TABLE pangya_coupon_desconto; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_coupon_desconto TO pangya;


--
-- TOC entry 5373 (class 0 OID 0)
-- Dependencies: 272
-- Name: TABLE pangya_course_cube_coin_temporada; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_course_cube_coin_temporada TO pangya;


--
-- TOC entry 5374 (class 0 OID 0)
-- Dependencies: 274
-- Name: TABLE pangya_course_drop; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_course_drop TO pangya;


--
-- TOC entry 5375 (class 0 OID 0)
-- Dependencies: 276
-- Name: TABLE pangya_course_drop_item; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_course_drop_item TO pangya;


--
-- TOC entry 5376 (class 0 OID 0)
-- Dependencies: 278
-- Name: TABLE pangya_course_reward_treasure; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_course_reward_treasure TO pangya;


--
-- TOC entry 5377 (class 0 OID 0)
-- Dependencies: 279
-- Name: TABLE pangya_cube_coin_location; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_cube_coin_location TO pangya;


--
-- TOC entry 5378 (class 0 OID 0)
-- Dependencies: 281
-- Name: TABLE pangya_daily_quest; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_daily_quest TO pangya;


--
-- TOC entry 5379 (class 0 OID 0)
-- Dependencies: 282
-- Name: TABLE pangya_daily_quest_player; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_daily_quest_player TO pangya;
GRANT INSERT ON TABLE pangya.pangya_daily_quest_player TO "pangyaPHP";


--
-- TOC entry 5380 (class 0 OID 0)
-- Dependencies: 283
-- Name: TABLE pangya_dolfini_locker; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_dolfini_locker TO pangya;
GRANT INSERT ON TABLE pangya.pangya_dolfini_locker TO "pangyaPHP";


--
-- TOC entry 5381 (class 0 OID 0)
-- Dependencies: 284
-- Name: TABLE pangya_dolfini_locker_item; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_dolfini_locker_item TO pangya;
GRANT SELECT ON TABLE pangya.pangya_dolfini_locker_item TO "pangyaPHP";


--
-- TOC entry 5382 (class 0 OID 0)
-- Dependencies: 286
-- Name: TABLE pangya_donation_epin; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_donation_epin TO pangya;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE pangya.pangya_donation_epin TO "pangyaPHP";


--
-- TOC entry 5383 (class 0 OID 0)
-- Dependencies: 288
-- Name: TABLE pangya_donation_item_log; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_donation_item_log TO pangya;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE pangya.pangya_donation_item_log TO "pangyaPHP";


--
-- TOC entry 5384 (class 0 OID 0)
-- Dependencies: 290
-- Name: TABLE pangya_donation_log; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_donation_log TO pangya;
GRANT SELECT ON TABLE pangya.pangya_donation_log TO "pangyaPHP";


--
-- TOC entry 5385 (class 0 OID 0)
-- Dependencies: 292
-- Name: TABLE pangya_donation_new; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_donation_new TO pangya;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE pangya.pangya_donation_new TO "pangyaPHP";


--
-- TOC entry 5386 (class 0 OID 0)
-- Dependencies: 294
-- Name: TABLE pangya_friend_list; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_friend_list TO pangya;


--
-- TOC entry 5387 (class 0 OID 0)
-- Dependencies: 295
-- Name: TABLE pangya_gacha_jp_all_item_list; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_gacha_jp_all_item_list TO pangya;
GRANT SELECT ON TABLE pangya.pangya_gacha_jp_all_item_list TO "pangyaPHP";


--
-- TOC entry 5388 (class 0 OID 0)
-- Dependencies: 297
-- Name: TABLE pangya_gacha_jp_item_list; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_gacha_jp_item_list TO pangya;
GRANT SELECT ON TABLE pangya.pangya_gacha_jp_item_list TO "pangyaPHP";


--
-- TOC entry 5389 (class 0 OID 0)
-- Dependencies: 299
-- Name: TABLE pangya_gacha_jp_player_win; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_gacha_jp_player_win TO pangya;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE pangya.pangya_gacha_jp_player_win TO "pangyaPHP";


--
-- TOC entry 5390 (class 0 OID 0)
-- Dependencies: 301
-- Name: TABLE pangya_gacha_jp_rate; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_gacha_jp_rate TO pangya;
GRANT SELECT ON TABLE pangya.pangya_gacha_jp_rate TO "pangyaPHP";


--
-- TOC entry 5391 (class 0 OID 0)
-- Dependencies: 302
-- Name: TABLE pangya_gift_table; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_gift_table TO pangya;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE pangya.pangya_gift_table TO "pangyaPHP";


--
-- TOC entry 5392 (class 0 OID 0)
-- Dependencies: 304
-- Name: TABLE pangya_gm_gift_web_log; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_gm_gift_web_log TO pangya;
GRANT SELECT,INSERT ON TABLE pangya.pangya_gm_gift_web_log TO "pangyaPHP";


--
-- TOC entry 5393 (class 0 OID 0)
-- Dependencies: 306
-- Name: TABLE pangya_golden_time_info; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_golden_time_info TO pangya;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE pangya.pangya_golden_time_info TO "pangyaPHP";


--
-- TOC entry 5394 (class 0 OID 0)
-- Dependencies: 308
-- Name: TABLE pangya_golden_time_item; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_golden_time_item TO pangya;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE pangya.pangya_golden_time_item TO "pangyaPHP";


--
-- TOC entry 5395 (class 0 OID 0)
-- Dependencies: 310
-- Name: TABLE pangya_golden_time_round; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_golden_time_round TO pangya;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE pangya.pangya_golden_time_round TO "pangyaPHP";


--
-- TOC entry 5396 (class 0 OID 0)
-- Dependencies: 312
-- Name: TABLE pangya_grand_zodiac_pontos; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_grand_zodiac_pontos TO pangya;
GRANT SELECT,INSERT,UPDATE ON TABLE pangya.pangya_grand_zodiac_pontos TO "pangyaPHP";


--
-- TOC entry 5397 (class 0 OID 0)
-- Dependencies: 313
-- Name: TABLE pangya_grand_zodiac_times; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_grand_zodiac_times TO pangya;


--
-- TOC entry 5398 (class 0 OID 0)
-- Dependencies: 315
-- Name: TABLE pangya_grandprix_clear; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_grandprix_clear TO pangya;


--
-- TOC entry 5399 (class 0 OID 0)
-- Dependencies: 317
-- Name: TABLE pangya_grandprix_event_config; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_grandprix_event_config TO pangya;


--
-- TOC entry 5400 (class 0 OID 0)
-- Dependencies: 319
-- Name: TABLE pangya_guild; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_guild TO pangya;
GRANT ALL ON TABLE pangya.pangya_guild TO "pangyaPHP";


--
-- TOC entry 5401 (class 0 OID 0)
-- Dependencies: 320
-- Name: TABLE pangya_guild_atividade_player; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_guild_atividade_player TO pangya;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE pangya.pangya_guild_atividade_player TO "pangyaPHP";


--
-- TOC entry 5402 (class 0 OID 0)
-- Dependencies: 322
-- Name: TABLE pangya_guild_bbs; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_guild_bbs TO pangya;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE pangya.pangya_guild_bbs TO "pangyaPHP";


--
-- TOC entry 5403 (class 0 OID 0)
-- Dependencies: 323
-- Name: TABLE pangya_guild_bbs_res; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_guild_bbs_res TO pangya;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE pangya.pangya_guild_bbs_res TO "pangyaPHP";


--
-- TOC entry 5404 (class 0 OID 0)
-- Dependencies: 327
-- Name: TABLE pangya_guild_intro_img_log; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_guild_intro_img_log TO pangya;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE pangya.pangya_guild_intro_img_log TO "pangyaPHP";


--
-- TOC entry 5405 (class 0 OID 0)
-- Dependencies: 329
-- Name: TABLE pangya_guild_mark_log; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_guild_mark_log TO pangya;


--
-- TOC entry 5406 (class 0 OID 0)
-- Dependencies: 331
-- Name: TABLE pangya_guild_match; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_guild_match TO pangya;
GRANT SELECT ON TABLE pangya.pangya_guild_match TO "pangyaPHP";


--
-- TOC entry 5407 (class 0 OID 0)
-- Dependencies: 333
-- Name: TABLE pangya_guild_member; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_guild_member TO pangya;
GRANT ALL ON TABLE pangya.pangya_guild_member TO "pangyaPHP";


--
-- TOC entry 5408 (class 0 OID 0)
-- Dependencies: 334
-- Name: TABLE pangya_guild_notice; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_guild_notice TO pangya;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE pangya.pangya_guild_notice TO "pangyaPHP";


--
-- TOC entry 5409 (class 0 OID 0)
-- Dependencies: 336
-- Name: TABLE pangya_guild_private_bbs; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_guild_private_bbs TO pangya;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE pangya.pangya_guild_private_bbs TO "pangyaPHP";


--
-- TOC entry 5410 (class 0 OID 0)
-- Dependencies: 337
-- Name: TABLE pangya_guild_private_bbs_res; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_guild_private_bbs_res TO pangya;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE pangya.pangya_guild_private_bbs_res TO "pangyaPHP";


--
-- TOC entry 5411 (class 0 OID 0)
-- Dependencies: 340
-- Name: TABLE pangya_guild_ranking; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_guild_ranking TO pangya;
GRANT SELECT ON TABLE pangya.pangya_guild_ranking TO "pangyaPHP";


--
-- TOC entry 5412 (class 0 OID 0)
-- Dependencies: 342
-- Name: TABLE pangya_guild_update_activity; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_guild_update_activity TO pangya;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE pangya.pangya_guild_update_activity TO "pangyaPHP";


--
-- TOC entry 5413 (class 0 OID 0)
-- Dependencies: 344
-- Name: TABLE pangya_gz_event_2016121600_rare_win; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_gz_event_2016121600_rare_win TO pangya;
GRANT SELECT,INSERT ON TABLE pangya.pangya_gz_event_2016121600_rare_win TO "pangyaPHP";


--
-- TOC entry 5414 (class 0 OID 0)
-- Dependencies: 346
-- Name: TABLE pangya_ip_table; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_ip_table TO pangya;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE pangya.pangya_ip_table TO "pangyaPHP";


--
-- TOC entry 5415 (class 0 OID 0)
-- Dependencies: 348
-- Name: TABLE pangya_item_buff; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_item_buff TO pangya;


--
-- TOC entry 5416 (class 0 OID 0)
-- Dependencies: 350
-- Name: TABLE pangya_item_mail; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_item_mail TO pangya;
GRANT SELECT,INSERT ON TABLE pangya.pangya_item_mail TO "pangyaPHP";


--
-- TOC entry 5417 (class 0 OID 0)
-- Dependencies: 351
-- Name: TABLE pangya_item_warehouse; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_item_warehouse TO pangya;
GRANT SELECT,UPDATE ON TABLE pangya.pangya_item_warehouse TO "pangyaPHP";


--
-- TOC entry 5418 (class 0 OID 0)
-- Dependencies: 353
-- Name: TABLE pangya_last_players_user; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_last_players_user TO pangya;
GRANT INSERT ON TABLE pangya.pangya_last_players_user TO "pangyaPHP";


--
-- TOC entry 5419 (class 0 OID 0)
-- Dependencies: 354
-- Name: TABLE pangya_last_up_clubset; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_last_up_clubset TO pangya;


--
-- TOC entry 5420 (class 0 OID 0)
-- Dependencies: 355
-- Name: TABLE pangya_login_reward; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_login_reward TO pangya;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE pangya.pangya_login_reward TO "pangyaPHP";


--
-- TOC entry 5421 (class 0 OID 0)
-- Dependencies: 357
-- Name: TABLE pangya_login_reward_player; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_login_reward_player TO pangya;


--
-- TOC entry 5422 (class 0 OID 0)
-- Dependencies: 359
-- Name: TABLE pangya_lucia_attendance; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_lucia_attendance TO pangya;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE pangya.pangya_lucia_attendance TO "pangyaPHP";


--
-- TOC entry 5423 (class 0 OID 0)
-- Dependencies: 360
-- Name: TABLE pangya_lucia_attendance_reward_log; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_lucia_attendance_reward_log TO pangya;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE pangya.pangya_lucia_attendance_reward_log TO "pangyaPHP";


--
-- TOC entry 5424 (class 0 OID 0)
-- Dependencies: 362
-- Name: TABLE pangya_mac_table; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_mac_table TO pangya;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE pangya.pangya_mac_table TO "pangyaPHP";


--
-- TOC entry 5425 (class 0 OID 0)
-- Dependencies: 364
-- Name: TABLE pangya_mascot_info; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_mascot_info TO pangya;
GRANT SELECT ON TABLE pangya.pangya_mascot_info TO "pangyaPHP";


--
-- TOC entry 5426 (class 0 OID 0)
-- Dependencies: 366
-- Name: TABLE pangya_memorial_shop_coin_item; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_memorial_shop_coin_item TO pangya;


--
-- TOC entry 5427 (class 0 OID 0)
-- Dependencies: 367
-- Name: TABLE pangya_memorial_shop_level; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_memorial_shop_level TO pangya;


--
-- TOC entry 5428 (class 0 OID 0)
-- Dependencies: 368
-- Name: TABLE pangya_memorial_shop_luckyset; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_memorial_shop_luckyset TO pangya;


--
-- TOC entry 5429 (class 0 OID 0)
-- Dependencies: 369
-- Name: TABLE pangya_memorial_shop_normal_item; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_memorial_shop_normal_item TO pangya;


--
-- TOC entry 5430 (class 0 OID 0)
-- Dependencies: 370
-- Name: TABLE pangya_memorial_shop_rare_item; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_memorial_shop_rare_item TO pangya;


--
-- TOC entry 5431 (class 0 OID 0)
-- Dependencies: 371
-- Name: TABLE pangya_memorial_shop_rate; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_memorial_shop_rate TO pangya;


--
-- TOC entry 5432 (class 0 OID 0)
-- Dependencies: 372
-- Name: TABLE pangya_msg_user; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_msg_user TO pangya;


--
-- TOC entry 5433 (class 0 OID 0)
-- Dependencies: 374
-- Name: TABLE pangya_myroom; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_myroom TO pangya;
GRANT INSERT ON TABLE pangya.pangya_myroom TO "pangyaPHP";


--
-- TOC entry 5434 (class 0 OID 0)
-- Dependencies: 375
-- Name: TABLE pangya_new_box; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_new_box TO pangya;


--
-- TOC entry 5435 (class 0 OID 0)
-- Dependencies: 377
-- Name: TABLE pangya_new_box_item; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_new_box_item TO pangya;


--
-- TOC entry 5436 (class 0 OID 0)
-- Dependencies: 379
-- Name: TABLE pangya_new_box_rare_win_log; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_new_box_rare_win_log TO pangya;


--
-- TOC entry 5437 (class 0 OID 0)
-- Dependencies: 381
-- Name: TABLE pangya_new_card_pack; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_new_card_pack TO pangya;


--
-- TOC entry 5438 (class 0 OID 0)
-- Dependencies: 383
-- Name: TABLE pangya_new_cards; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_new_cards TO pangya;


--
-- TOC entry 5439 (class 0 OID 0)
-- Dependencies: 385
-- Name: TABLE pangya_new_course_drop; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_new_course_drop TO pangya;


--
-- TOC entry 5440 (class 0 OID 0)
-- Dependencies: 387
-- Name: TABLE pangya_new_course_drop_item; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_new_course_drop_item TO pangya;


--
-- TOC entry 5441 (class 0 OID 0)
-- Dependencies: 389
-- Name: TABLE pangya_new_memorial_level; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_new_memorial_level TO pangya;


--
-- TOC entry 5442 (class 0 OID 0)
-- Dependencies: 390
-- Name: TABLE pangya_new_memorial_lucky_set; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_new_memorial_lucky_set TO pangya;


--
-- TOC entry 5443 (class 0 OID 0)
-- Dependencies: 392
-- Name: TABLE pangya_new_memorial_normal_item; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_new_memorial_normal_item TO pangya;


--
-- TOC entry 5444 (class 0 OID 0)
-- Dependencies: 394
-- Name: TABLE pangya_new_memorial_rare_win_log; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_new_memorial_rare_win_log TO pangya;


--
-- TOC entry 5445 (class 0 OID 0)
-- Dependencies: 396
-- Name: TABLE pangya_notice_list; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_notice_list TO pangya;
GRANT SELECT,INSERT ON TABLE pangya.pangya_notice_list TO "pangyaPHP";


--
-- TOC entry 5446 (class 0 OID 0)
-- Dependencies: 398
-- Name: TABLE pangya_papel_shop_config; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_papel_shop_config TO pangya;


--
-- TOC entry 5447 (class 0 OID 0)
-- Dependencies: 399
-- Name: TABLE pangya_papel_shop_coupon; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_papel_shop_coupon TO pangya;


--
-- TOC entry 5448 (class 0 OID 0)
-- Dependencies: 401
-- Name: TABLE pangya_papel_shop_info; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_papel_shop_info TO pangya;
GRANT INSERT ON TABLE pangya.pangya_papel_shop_info TO "pangyaPHP";


--
-- TOC entry 5449 (class 0 OID 0)
-- Dependencies: 403
-- Name: TABLE pangya_papel_shop_item; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_papel_shop_item TO pangya;


--
-- TOC entry 5450 (class 0 OID 0)
-- Dependencies: 405
-- Name: TABLE pangya_papel_shop_rare_win_log; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_papel_shop_rare_win_log TO pangya;


--
-- TOC entry 5451 (class 0 OID 0)
-- Dependencies: 407
-- Name: TABLE pangya_parts_list; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_parts_list TO pangya;


--
-- TOC entry 5452 (class 0 OID 0)
-- Dependencies: 409
-- Name: TABLE pangya_personal_shop_log; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_personal_shop_log TO pangya;


--
-- TOC entry 5453 (class 0 OID 0)
-- Dependencies: 411
-- Name: TABLE pangya_player_ip; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_player_ip TO pangya;
GRANT SELECT,INSERT ON TABLE pangya.pangya_player_ip TO "pangyaPHP";


--
-- TOC entry 5454 (class 0 OID 0)
-- Dependencies: 413
-- Name: TABLE pangya_player_location; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_player_location TO pangya;
GRANT SELECT,INSERT ON TABLE pangya.pangya_player_location TO "pangyaPHP";


--
-- TOC entry 5455 (class 0 OID 0)
-- Dependencies: 414
-- Name: TABLE pangya_quest; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_quest TO pangya;


--
-- TOC entry 5456 (class 0 OID 0)
-- Dependencies: 415
-- Name: TABLE pangya_quest_clear; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_quest_clear TO pangya;


--
-- TOC entry 5457 (class 0 OID 0)
-- Dependencies: 421
-- Name: TABLE pangya_rank_atual_character; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_rank_atual_character TO pangya;


--
-- TOC entry 5458 (class 0 OID 0)
-- Dependencies: 423
-- Name: TABLE pangya_rank_config; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_rank_config TO pangya;


--
-- TOC entry 5459 (class 0 OID 0)
-- Dependencies: 425
-- Name: TABLE pangya_record; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_record TO pangya;


--
-- TOC entry 5460 (class 0 OID 0)
-- Dependencies: 426
-- Name: TABLE pangya_rescue_pwd_log; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_rescue_pwd_log TO pangya;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE pangya.pangya_rescue_pwd_log TO "pangyaPHP";


--
-- TOC entry 5461 (class 0 OID 0)
-- Dependencies: 428
-- Name: TABLE pangya_reward_ssc; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_reward_ssc TO pangya;


--
-- TOC entry 5462 (class 0 OID 0)
-- Dependencies: 429
-- Name: TABLE pangya_scratchy_prob_sec; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_scratchy_prob_sec TO pangya;
GRANT INSERT ON TABLE pangya.pangya_scratchy_prob_sec TO "pangyaPHP";


--
-- TOC entry 5463 (class 0 OID 0)
-- Dependencies: 430
-- Name: TABLE pangya_server_list; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_server_list TO pangya;
GRANT SELECT ON TABLE pangya.pangya_server_list TO "pangyaPHP";


--
-- TOC entry 5464 (class 0 OID 0)
-- Dependencies: 431
-- Name: TABLE pangya_shutdown_list; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_shutdown_list TO pangya;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE pangya.pangya_shutdown_list TO "pangyaPHP";


--
-- TOC entry 5465 (class 0 OID 0)
-- Dependencies: 433
-- Name: TABLE pangya_ticker_list; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_ticker_list TO pangya;


--
-- TOC entry 5466 (class 0 OID 0)
-- Dependencies: 435
-- Name: TABLE pangya_ticket_report; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_ticket_report TO pangya;


--
-- TOC entry 5467 (class 0 OID 0)
-- Dependencies: 436
-- Name: TABLE pangya_ticket_report_dados; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_ticket_report_dados TO pangya;


--
-- TOC entry 5468 (class 0 OID 0)
-- Dependencies: 438
-- Name: TABLE pangya_tiki_points; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_tiki_points TO pangya;
GRANT INSERT ON TABLE pangya.pangya_tiki_points TO "pangyaPHP";


--
-- TOC entry 5469 (class 0 OID 0)
-- Dependencies: 439
-- Name: TABLE pangya_transforme_clubset_temp; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_transforme_clubset_temp TO pangya;


--
-- TOC entry 5470 (class 0 OID 0)
-- Dependencies: 441
-- Name: TABLE pangya_treasure_item; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_treasure_item TO pangya;


--
-- TOC entry 5471 (class 0 OID 0)
-- Dependencies: 442
-- Name: TABLE pangya_trofel_especial; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_trofel_especial TO pangya;


--
-- TOC entry 5472 (class 0 OID 0)
-- Dependencies: 444
-- Name: TABLE pangya_trofel_grandprix; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_trofel_grandprix TO pangya;


--
-- TOC entry 5473 (class 0 OID 0)
-- Dependencies: 446
-- Name: TABLE pangya_user_equip; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_user_equip TO pangya;
GRANT INSERT ON TABLE pangya.pangya_user_equip TO "pangyaPHP";


--
-- TOC entry 5474 (class 0 OID 0)
-- Dependencies: 447
-- Name: TABLE pangya_user_macro; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_user_macro TO pangya;
GRANT INSERT ON TABLE pangya.pangya_user_macro TO "pangyaPHP";


--
-- TOC entry 5475 (class 0 OID 0)
-- Dependencies: 448
-- Name: TABLE pangya_weblink_key; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.pangya_weblink_key TO pangya;
GRANT SELECT,INSERT,DELETE,UPDATE ON TABLE pangya.pangya_weblink_key TO "pangyaPHP";


--
-- TOC entry 5476 (class 0 OID 0)
-- Dependencies: 450
-- Name: TABLE td_char_equip_s4; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.td_char_equip_s4 TO pangya;


--
-- TOC entry 5477 (class 0 OID 0)
-- Dependencies: 452
-- Name: TABLE td_room_data; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.td_room_data TO pangya;
GRANT INSERT ON TABLE pangya.td_room_data TO "pangyaPHP";


--
-- TOC entry 5478 (class 0 OID 0)
-- Dependencies: 454
-- Name: TABLE trofel_stat; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.trofel_stat TO pangya;
GRANT INSERT ON TABLE pangya.trofel_stat TO "pangyaPHP";


--
-- TOC entry 5479 (class 0 OID 0)
-- Dependencies: 455
-- Name: TABLE tu_ucc; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.tu_ucc TO pangya;
GRANT SELECT ON TABLE pangya.tu_ucc TO "pangyaPHP";


--
-- TOC entry 5480 (class 0 OID 0)
-- Dependencies: 456
-- Name: TABLE tutorial; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.tutorial TO pangya;
GRANT INSERT ON TABLE pangya.tutorial TO "pangyaPHP";


--
-- TOC entry 5481 (class 0 OID 0)
-- Dependencies: 457
-- Name: TABLE user_info; Type: ACL; Schema: pangya; Owner: postgres
--

GRANT ALL ON TABLE pangya.user_info TO pangya;
GRANT SELECT,INSERT,UPDATE ON TABLE pangya.user_info TO "pangyaPHP";


-- Completed on 2021-10-19 09:02:17

--
-- PostgreSQL database dump complete
--

