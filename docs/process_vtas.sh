#!/bin/bash

# Process VTAs generated from Lead-DBS in STANDARD (MNI-152) space
# Usage: run from WSL with FSL in PATH (source FSL first if needed)

set -e

# Set paths (WSL path to Windows D: drive)
VTA_DIR='/mnt/d/lead-dbs-dystonia/outputs/leaddbsreg01/derivatives/leaddbs/sub-DBSREG001/stimulations/MNI152NLin2009bAsym/20250918142202'
FSL_REF="${FSLDIR:-/home/capit/fsl}/data/standard/MNI152_T1_1mm_brain.nii.gz"

if [[ ! -d "$VTA_DIR" ]]; then
    echo "Error: VTA_DIR does not exist: $VTA_DIR"
    exit 1
fi

if [[ ! -f "$FSL_REF" ]]; then
    echo "Error: FSL reference not found: $FSL_REF"
    echo "Set FSLDIR or install FSL and source fsl.sh"
    exit 1
fi

# Loop over subdirectories only (each subject/session folder)
for subject_dir in "$VTA_DIR"/*/; do
    subject=$(basename "$subject_dir")
    echo "Processing VTAs for $subject"

    # Resolve single file per hemisphere (globs can match multiple)
    VTA_LEFT=$(ls "$VTA_DIR/$subject"/*hemi-L.nii 2>/dev/null | head -1)
    VTA_RIGHT=$(ls "$VTA_DIR/$subject"/*hemi-R.nii 2>/dev/null | head -1)

    if [[ -z "$VTA_LEFT" ]]; then
        echo "  Skip: no *hemi-L.nii in $subject"
        continue
    fi
    if [[ -z "$VTA_RIGHT" ]]; then
        echo "  Skip: no *hemi-R.nii in $subject"
        continue
    fi

    VTA_LEFT_MNI="$VTA_DIR/$subject/vta_left_mni.nii"
    VTA_RIGHT_MNI="$VTA_DIR/$subject/vta_right_mni.nii"
    VTA_COMBINED="$VTA_DIR/$subject/vtas_mni.nii"

    # Transform left VTA to MNI space and binarise (use temp to avoid read/write same file)
    flirt -in "$VTA_LEFT" -ref "$FSL_REF" -out "$VTA_LEFT_MNI" -applyxfm -usesqform
    fslmaths "$VTA_LEFT_MNI" -bin "${VTA_LEFT_MNI}.tmp"
    mv "${VTA_LEFT_MNI}.tmp" "$VTA_LEFT_MNI"

    flirt -in "$VTA_RIGHT" -ref "$FSL_REF" -out "$VTA_RIGHT_MNI" -applyxfm -usesqform
    fslmaths "$VTA_RIGHT_MNI" -bin "${VTA_RIGHT_MNI}.tmp"
    mv "${VTA_RIGHT_MNI}.tmp" "$VTA_RIGHT_MNI"

    fslmaths "$VTA_LEFT_MNI" -add "$VTA_RIGHT_MNI" "$VTA_COMBINED"
    echo "  Wrote $VTA_COMBINED"
done

echo "Done."
