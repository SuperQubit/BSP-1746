		AREA    ComodoDriveBinFile_Section, DATA, READONLY, ALIGN=8
        EXPORT  ComodoDriveBinaryImage

; Includes the binary file ComodoDrive.bin from the current source folder
ComodoDriveBinaryImage
        INCBIN  ComodoDrive.bin
ComodoDriveBinaryImage_End

;  Fill remaining image space with 0xFF
		FILL  32768-(ComodoDriveBinaryImage_End - ComodoDriveBinaryImage),0xFF
		
; define a constant which contains the size of the image above
ComodoDriveBinaryImage_length
        DCD     ComodoDriveBinaryImage_End - ComodoDriveBinaryImage

        EXPORT  ComodoDriveBinaryImage_length

ComodoDriveImageManifest
		DCB		"533Comodo HFOC 2.13 SDK",0
		EXPORT  ComodoDriveImageManifest
        END
			