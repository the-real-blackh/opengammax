
This disk holds the I.A.E.A. Intercomparison Spectra.

The spectra are:

G1100 - Reference Spectrum. All peaks in other spectra are derived from this
	spectrum. The data sheet gives peak shift relative to this spectrum
	and peak area ratio relative to peaks in this spectrum.
G1200 - Peak search test.
G1300 to G1305 - Consistency test. Spectra are identical in composition but
	with differing statistical scatter.
G1400 - Deconvolution test.


The spectra are of 2000 channels and are given in four formats:

.CHN spectra are in EG&G binary format thus:
  32 byte header information
  2000 channels, 4 bytes per channel, spectral information. Only the bottom
  three bytes are significant.
  48 null channels to fill out to 2048 channel spectrum size.
  EG&G calibration and labelling information.
Most users can ignore the header and calibration information entirely.

.ASC files are ASCII text files comprising a short text header followed
  by a list of channel contents, 8 channels per line.

.MCA files are binary files in Canberra S100 format thus:
  128 bytes of header and information.
  2000 channels, 4 bytes per channel, spectral information padded out to 2048
  channels with null channels.

.SPM files are binary files in OXFORD format. These files have been converted
  from .CHN format by the OXFORD conversion program, who's name I forget.
  I am not certain that all the information held in the CHN format is carried
  over into the SPM files.
  Format not known to me.

USING THE SPECTRA to test your software.
----------------------------------------

a) Analyse the G1100 spectrum. This contains the reference peaks and your
analysis should provide you with a list of peak areas and peak positions.
Since these reference peaks are well defined with a large number of counts
there should be little uncertainty on these peak positions and areas.

All the peaks in the other spectra were produced by shifting one or other
of these reference peaks by an INTEGRAL number of channels and dividing
by an attenuation factor. Each channel was then adjusted by the imposition of
a valid statistical uncertainty in such a way that neither positions or areas
were altered in any way. These shifts and factors are given on the
accompanying sheet. You know, therefore, precisely where the peaks should
be in the G1200-G1400 spectra and precisely what the peak area should be.

b) Analyse each spectrum in turn and compared the measured positions and areas
with the expected values. Ideally your software should detect all peaks at
a position given by (Reference position +/- SHIFT) and with a peak area of
(Reference Area/attenuation factor). Your software should not report any peaks
which do not exist. (You may find that a spurious peak at around channel
1024 may appear in those spectra which have a large change in background
level. This can be ignored as it is not a normal spectrum feature.)

The disk is accompanied by a copy of the shift and attenuation factors released,
by the I.A.E.A. after the original intercomparison exercise. Analyses of that
exercise can be found in the following articles:

R.M.Parr, H.Houtermans and J.Schaerf 'The IAEA Intercomparison of Methods
for processing Ge(Li) gamma ray spectra.' in 'Computers in Activation Analysis
and Gamma Ray Spectrometry' Ed. Carpenter et.al.
I.A.E.A. CONF-780421 (1979), p544.

P.Zagyvai, R.M.Parr, L.Gy.Nagy 'Additional results for the G-1 IAEA Inter-
comparison ... etc.', J.Radioanal.Nucl.Chem., 89(1985)589-607.

Nielsen, S.P., Quantitative evaluation of gamma-spectrum analysis methods using 
IAEA test spectra.  Nucl. Instrum. Methods 192 (1982) 433-438.

Spreadsheets and library files for some common spectrum analysis 
programs are available to facilitate the use of these spectra.  These
can be obtained at small cost from:



Dr Gordon Gilmore
Nuclear Training Services Ltd.
6 Alexandra Road,
Grappenhall,
Warrington WA4 2EL
01925-604232

Email: mail@nts.demon.co.uk
http://www.nts.demon.co.uk
