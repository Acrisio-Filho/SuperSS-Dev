<?php
    // Arquivo util.inc
    // Criado em 28/06/2020 as 17:50 por Acrisio
    // Definição dos utilitário para o Event System

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
?>