/*
Fsseudonymizer - customprocessing.h/cpp
---------------------------------------
Begin: 2019/10/13
Author: Aaron Reeves <aaron.reeves@sruc.ac.uk>
---------------------------------------------------
Copyright (C) 2019 Scotland's Rural College (SRUC)

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General
Public License as published by the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.
*/

#include <ar_general_purpose/returncodes.h>
#include <ar_general_purpose/ctwodarray.h>

int customPreProcessData( CTwoDArray<QVariant>* data );

int customPreProcessRow( CTwoDArray<QVariant>* data, const int rowIdx );

int customPostProcessRow( CTwoDArray<QVariant>* pseudonymizedData, const int rowIdx );

int customPostProcessData( CTwoDArray<QVariant>* pseudonymizedData );

