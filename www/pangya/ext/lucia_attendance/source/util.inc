<?php
    // Arquivo util.inc
    // Criado em 28/02/2020 as 08:15 por Acrisio
    // Definição das funções de suporte para as classes do Sistema Lucia Attendance

    // Get Card Rarity Name short
    function getRarityBref($rarity) {

        switch ($rarity) {
            case 0:
                return "N";
            case 1:
                return "R";
            case 2:
                return "SR";
            case 3:
                return "SC";
        }
    
        return "Unknown";
    }

    // Get Item Group Type
    function getItemGroupType($typeid) {
        return ($typeid & 0xFC000000) >> 26;
    }
    
    // Get Item Type Name
    function getItemTypeName($type) {
    
        switch ($type) {
            case 2:	// Part
                return "Outfit";
            case 4:
                return "Clubset";
            case 6:
                return "Item";
            case 7:
                return "Caddie";
            case 9:
                return "Setitem";
            case 16:
                return "Mascot";
            case 28:
                return "Ring";
            case 31:
                return "Card";
        }
    
        return "Unknown";
    }
    
    // Get Card Group Type
    function getCardGroupType($typeid) {
        return ($typeid & 0x03FF0000) >> 22;
    }
    
    // Get Card Type Name
    function getCardTypeName($type) {
    
        switch ($type) {
            case 0:
                return "Character";
            case 1:
                return "Caddie";
            case 2:
                return "Special";
            case 5:
                return "NPC";
        }
    
        return "Unknown";
    }

    // Finds

    // find_if
    function find_if($arr, $func) {

        if ($func && isset($arr) && (is_array($arr) || is_object($arr)) && count($arr) > 0) {
    
            foreach ($arr as $k => &$el) {
                
                if ($func($el))
                    return $el;
            }
        }
    
        return null;
    }
    
    // exists_if
    function exists_if($arr, $func) {
    
        if ($func && isset($arr) && (is_array($arr) || is_object($arr) && count($arr) > 0)) {
    
            foreach ($arr as $k => &$el) {
    
                if ($func($el))
                    return true;
            }
        }
    
        return false;
    }
    
    // Find Item By Category
    function findItemByCategory($all, $item) {
    
        $obj = find_if($all, function($el) use(&$item) {
            return property_exists($el, $item->type);
        });
    
        if ($obj && (is_array($obj) || is_object($obj))) {
            return find_if(((Array)$obj)[$item->type], function($el) use (&$item) {
                return $el->_typeid == $item->_typeid;
            });
        }
    
        return null;
    }
    
    // Find Card By Category
    function findCardByCategory($all, $card) {
    
        foreach ($all as &$e) {
    
            foreach ($e as &$ez) {
    
                $obj = find_if($ez, function($el) use(&$card) {
                    return property_exists($el, $card->type);
                });
    
                if ($obj && (is_array($obj) || is_object($obj))) {
                    if (($cc = find_if(((Array)$obj)[$card->type], function($el) use(&$card) {
                        return $el->_typeid == $card->_typeid;
                    })) != null)
                        return $cc;
                }
            }
        }
    
        return null;
    }
?>