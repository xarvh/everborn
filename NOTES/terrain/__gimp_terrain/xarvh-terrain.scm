

(define (script-fu-xarvh-terrain
    inBaseColor
    inOverColor
    inTurbolence
    inMinimum
    inSeed)
    (let* (
	    (theImageW 64)
	    (theImageH 64)
	    (theImage (car (gimp-image-new theImageW theImageH RGB)))
	    (theBaseL (car (gimp-layer-new theImage theImageW theImageH RGB "Base" 100 NORMAL) ))
	    (theOverL (car (gimp-layer-new theImage theImageW theImageH RGB "Over" 100 NORMAL) ))
	    (theMask)
	  )

    (gimp-image-add-layer theImage theBaseL 0)
    (gimp-image-add-layer theImage theOverL 0)

    ; BASE
    (plug-in-plasma TRUE theImage theBaseL inSeed inTurbolence)
    (plug-in-colorify TRUE theImage theBaseL inBaseColor)
    (gimp-levels theBaseL 0 0 255 1.0 inMinimum 255)

    (plug-in-plasma TRUE theImage theOverL (+ 789 inSeed) inTurbolence)
    (plug-in-colorify TRUE theImage theOverL inOverColor)
    (gimp-levels theOverL 0 0 255 1.0 inMinimum 255)

    (gimp-edit-copy theOverL)
    (set! theMask (car (gimp-layer-create-mask theOverL 5)))
    (gimp-layer-add-alpha theOverL)
    (gimp-layer-add-mask theOverL theMask)
    (gimp-image-merge-visible-layers theImage 0)

    (gimp-image-clean-all theImage)
    (gimp-display-new theImage)
    )
)


(script-fu-register "script-fu-xarvh-terrain"
    _"<Toolbox>/Terrain"
    "Xarvh Terrain"
    "Francesco Orsenigo"
    "Francesco Orsenigo"
    "??"
    ""
    SF-COLOR		_"Base Color"	'(0  0 255)
    SF-COLOR		_"Over Color"	'(6 255 255)
    SF-ADJUSTMENT	_"Turbolence"	'(3.5 0.0 7.0   0.5 0.5   1 0)
    SF-ADJUSTMENT	_"Minimum"	'(60 0 255    5 5    0 0)
    SF-ADJUSTMENT	_"Seed"		'(16681 0 200000000    100 100    0 0)
    )


;EOF
