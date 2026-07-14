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

# VTAs are in VTA_DIR directly (no subdirs): *hemi-L.nii and *hemi-R.nii pairs
shopt -s nullglob
for VTA_LEFT in "$VTA_DIR"/*hemi-L.nii; do
    stem=$(basename "$VTA_LEFT" _hemi-L.nii)
    VTA_RIGHT="$VTA_DIR/${stem}_hemi-R.nii"

    if [[ ! -f "$VTA_RIGHT" ]]; then
        echo "  Skip: no matching right hemisphere for $stem"
        continue
    fi

    echo "Processing VTAs for $stem"

    VTA_LEFT_MNI="$VTA_DIR/${stem}_vta_left_mni.nii"
    VTA_RIGHT_MNI="$VTA_DIR/${stem}_vta_right_mni.nii"
    VTA_COMBINED="$VTA_DIR/${stem}_vtas_mni.nii"

    # Transform left VTA to MNI space and binarise (temp file: FSL may write .nii or .nii.gz)
    flirt -in "$VTA_LEFT" -ref "$FSL_REF" -out "$VTA_LEFT_MNI" -applyxfm -usesqform
    TMP_LEFT="$VTA_DIR/${stem}_left_bin"
    TMP_RIGHT="$VTA_DIR/${stem}_right_bin"
    fslmaths "$VTA_LEFT_MNI" -bin "$TMP_LEFT"
    if [[ -f "${TMP_LEFT}.nii" ]]; then
        mv "${TMP_LEFT}.nii" "$VTA_LEFT_MNI"
    elif [[ -f "${TMP_LEFT}.nii.gz" ]]; then
        gunzip -c "${TMP_LEFT}.nii.gz" > "$VTA_LEFT_MNI"
        rm -f "${TMP_LEFT}.nii.gz"
    fi

    flirt -in "$VTA_RIGHT" -ref "$FSL_REF" -out "$VTA_RIGHT_MNI" -applyxfm -usesqform
    fslmaths "$VTA_RIGHT_MNI" -bin "$TMP_RIGHT"
    if [[ -f "${TMP_RIGHT}.nii" ]]; then
        mv "${TMP_RIGHT}.nii" "$VTA_RIGHT_MNI"
    elif [[ -f "${TMP_RIGHT}.nii.gz" ]]; then
        gunzip -c "${TMP_RIGHT}.nii.gz" > "$VTA_RIGHT_MNI"
        rm -f "${TMP_RIGHT}.nii.gz"
    fi

    fslmaths "$VTA_LEFT_MNI" -add "$VTA_RIGHT_MNI" "$VTA_COMBINED"
    echo "  Wrote $VTA_COMBINED"
done

echo "Done."
