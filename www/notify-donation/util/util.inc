<?php
    // Arquivo util.php
    // Criado 01/12/2020 as 14:03 por Acrisio
    
    /**
     * Class XmlValidator
     * @author Francesco Casula <fra.casula@gmail.com>
     */
    class XmlValidator
    {
        /**
         * @param string $xmlFilename Path to the XML file
         * @param string $version 1.0
         * @param string $encoding utf-8
         * @return bool
         */
        public function isXMLFileValid($xmlFilename, $version = '1.0', $encoding = 'utf-8')
        {
            $xmlContent = file_get_contents($xmlFilename);
            return $this->isXMLContentValid($xmlContent, $version, $encoding);
        }

        /**
         * @param string $xmlContent A well-formed XML string
         * @param string $version 1.0
         * @param string $encoding utf-8
         * @return bool
         */
        public function isXMLContentValid($xmlContent, $version = '1.0', $encoding = 'utf-8')
        {
            if (trim($xmlContent) == '') {
                return false;
            }

            libxml_use_internal_errors(true);

            $doc = new DOMDocument($version, $encoding);
            $doc->loadXML($xmlContent);

            $errors = libxml_get_errors();
            libxml_clear_errors();

            return empty($errors);
        }
    }

    // Singleton
    class sXmlValidator {

        static private $validator = null;

        static public function &getInstance() {

            if (self::$validator == null)
                self::$validator = new XmlValidator();

            return self::$validator;
        }
    }

    // Simple XML to Object
    function simpleXMLToObject($str_xml) {

        $xml = simplexml_load_string($str_xml, null, LIBXML_NOCDATA);

        if ($xml == null)
            return null;
        
        $json = json_encode($xml);

        return json_decode($json, FALSE);
    }

    // Simple Array to Object
    function simpleArrayToObject($array) {

        if (!is_array($array))
            return null;

        $json = json_encode($array);

        return json_decode($json, FALSE);
    }

    // Array Find
    function array_find($xs, $f) {
        foreach ($xs as $x) {
            if (call_user_func($f, $x) === true)
            return $x;
        }
        return null;
    }

    // Convert Array to XML
    function array_to_xml( $data, &$xml_data ) {
            
        foreach( $data as $key => $value ) {
            if( is_array($value) ) {
                if( is_numeric($key) ){
                    $key = 'item'.$key; //dealing with <0/>..<n/> issues
                }
                $subnode = $xml_data->addChild($key);
                array_to_xml($value, $subnode);
            } else if ($value != null) {
                $xml_data->addChild("$key",htmlspecialchars("$value"));
            }
        }
    }

    // Object to XML
    function objectToSimpleXML($obj) {

        if ($obj == null || !is_object($obj))
            return null;

        $data = json_decode(json_encode($obj), TRUE);

        // creating object of SimpleXMLElement
        $xml_data = new SimpleXMLElement('<?xml version="1.0"?><data></data>');

        // function call to convert array to xml
        array_to_xml($data, $xml_data);

        // return
        return $xml_data->asXML();
    }
?>